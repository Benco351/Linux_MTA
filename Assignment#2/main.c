#include "Utilities.h"

int main() {
    int pipeToChild[2]; // Pipe for sending numbers from parent to child
    int pipeToParent[2]; // Pipe for sending signals from child to parent
    int number, result;
    pid_t pid;
    DB* dataBase;
    bool first = true;

    if (pipe(pipeToChild) == -1 || pipe(pipeToParent) == -1) {
        fprintf(stderr, "Pipe failed.\n");
        return 1;
    }

    pid = fork(); // Create a child process

    if (pid < 0) {
        fprintf(stderr, "Fork failed.\n");
        return 1;
    }

    if (pid > 0) {
        signal(SIGUSR1, graceful_exit_handler);
        signal(SIGINT, sigint_handler);

        while (true) {
            printMenu();
            scanf("%d", &number);

            if (number < 1 || number > 7)
            {
                printf("Invalid choice, please try again.\n");
                continue;
            }

            write(pipeToChild[1], &number, sizeof(number)); // Write the number to the pipe

            if (number >= 1 && number <= 3)
            {
                int inputArrSize = 0, outputArrSize = 0, currentStringSize = 0;
                char** output = NULL, **input = NULL;
                input = readInput(&inputArrSize);
                write(pipeToChild[1], &inputArrSize, sizeof(int));

                ReadOrWriteToPipe(input, inputArrSize, pipeToChild, WRITE);
                read(pipeToParent[0], &outputArrSize, sizeof(int));
                output = (char**)malloc(sizeof(char*) * outputArrSize);
                checkAllocation(output);
                ReadOrWriteToPipe(output, outputArrSize, pipeToParent, READ);

                for (int i = 0; i < outputArrSize; i++)
                {
                    printf("%s", output[i]);
                    free(output[i]);
                }

                free(output);

                for (int i = 0; i < inputArrSize; i++)
                {
                    free(input[i]);
                }

                free(input);

            }

            if (number == 7)
                exit(EXIT_SUCCESS);

            read(pipeToParent[0], &result, sizeof(result)); // Wait for signal from the child

            printf("Received result: %d\n", result);
        }

        close(pipeToChild[1]); // Close the write end of the pipe for numbers
        close(pipeToParent[0]); // Close the read end of the pipe for signals

        wait(NULL); // Wait for the child process to finish
    } else if (pid == 0) {
        // Child process
        printf("1\n");
        if (first)
        {
            printf("2\n");
            //check if zip file exists; if it does, unzip it. if it doesn't, move on.

            if (true) //if file exists
            {
                printf("3\n");
                //unzip
                char** dirList = NULL;
                int listSize = 0;
                dirList = createDirList(&listSize);
                dataBase = getDataBase(listSize, dirList);
                printf("Im here?\n");
            }

            first = false;
        }

        while (true) {
            printf("Or am I here!\n");
            read(pipeToChild[0], &number, sizeof(number)); // Read the number from the pipe
            printf("Did I read the pipe?\n");
            child_process(pipeToChild, pipeToParent, number, dataBase);
        }

        close(pipeToChild[0]); // Close the read end of the pipe for numbers
        close(pipeToParent[1]); // Close the write end of the pipe for signals

        exit(0); // Terminate the child process
    }

    return 0;
}
