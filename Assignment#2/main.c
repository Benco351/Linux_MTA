#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Utilities.h"
#define CHILD_ID 6

void child_process(int pipefd[2], int choice) {
    close(pipefd[0]); // Close the read end of the pipe

    int result = 0;

    switch (choice)
    {
        case 1:
            receive_airport_icao(pipefd[1]);
            break;
        case 2:
            receive_airport_name(pipefd[1]);
            break;
        case 3:
            receive_aircraft_names(pipefd[1]);
            break;
        case 4:
            update_airports(pipefd[1]);
            break;
        case 5:
            zip_db_files(pipefd[1]);
            break;
        case 6:
            result = getpid();
            write(pipefd[1], &result, sizeof(int));
            break;
        case 7:
            printf("Graceful exit signal received\n");
            zip_db_files(pipefd[1]);
            exit(0);
        default:
            printf("Invalid choice.\n");
            result = -1; // Indicate an invalid choice
            write(pipefd[1], &result, sizeof(int));
            break;
    }

    close(pipefd[1]); // Close the write end of the pipe
    exit(0);
}

void graceful_exit_handler(int signum) {
    if (signum == SIGUSR1)
    {
        printf("Graceful exit signal received\n");
        zip_db_files(); // Assuming this function takes care of the DB zipping
        exit(0);
    }
}

void sigint_handler(int signum)
{
    if (signum == SIGINT) {
        printf("CTRL+C signal received\n");
        kill(getpid(), SIGUSR1);
    }
}

int main()
{
    int choice;
    int pipefd[2]; // File descriptors for the pipe
    int cidpipe[2]; // File descriptors for child process communication

    if (pipe(pipefd) == -1 || pipe(cidpipe) == -1)
    {
        fprintf(stderr, "Failed to create pipe.\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process
        close(pipefd[0]); // Close unused read end of the main process pipe
        close(cidpipe[1]); // Close unused write end of the child process pipe

        read(cidpipe[0], &choice, sizeof(int)); // Read the choice from the main process

        child_process(pipefd, choice);
    }
    else if (pid > 0)
    {
        // Main process
        signal(SIGUSR1, graceful_exit_handler);
        signal(SIGINT, sigint_handler);

        close(pipefd[1]); // Close unused write end of the main process pipe
        close(cidpipe[0]); // Close unused read end of the child process pipe

        while (1) {
            printf("Menu:\n");
            printf("1. Receive airports ICOA code names and output all arrival flights and flight details.\n");
            printf("2. Receive airports name and output the full airport schedule (departures and arrivals) ordered by time.\n");
            printf("3. Receive list of aircraft names (icao24) and output all flights (departures and arrivals) that it has made.\n");
            printf("4. Update the existing airports in the DB with recent data (rerun BASH script).\n");
            printf("5. Zip the DB files.\n");
            printf("6. Get child process's ID.\n");
            printf("7. Graceful exit - child shall zip the DB files and terminate (think about collecting the exit status of the child process).\n");
            printf("Enter your choice (1-7): ");

            scanf("%d", &choice);

            if (choice == CHILD_ID)
            {
                write(cidpipe[1], &choice, sizeof(int));
            }
            else
            {
                write(pipefd[1], &choice, sizeof(int));
            }

            int child_status;
            if (waitpid(pid, &child_status, WNOHANG) > 0)
            {
                printf("Child process terminated with status: %d\n", WEXITSTATUS(child_status));
                break;
            }
            
            int result;
            if (choice == CHILD_ID)
            {
                read(pipefd[0], &result, sizeof(int));
                close(pipefd[0]);
            }
            else
            {
                read(cidpipe[1], &result, sizeof(int));
                close(cidpipe[1]);
            }

            printf("Received result for operation %d: %d\n", choice, result);
        }
    }
    else
    {
        // Fork failed
        fprintf(stderr, "Failed to create child process.\n");
        return 1;
    }

    return 0;
}
