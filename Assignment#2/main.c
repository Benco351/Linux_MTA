#include "Utilities.h"

int parentBytesCounter = 0, childBytesCounter = 0;

int main() 
{
    int pipeToChild[2]; // Pipe for sending numbers from parent to child
    int pipeToParent[2]; // Pipe for sending signals from child to parent
    int number, result;
    pid_t pid, childPID;
    DB* dataBase = NULL;
    bool firstChildIteration = true;

    if (pipe(pipeToChild) == -1 || pipe(pipeToParent) == -1) 
    {
        fprintf(stderr, "Pipe failed.\n");
        return 1;
    }

    pid = fork(); // Create a child process

    if (pid < 0) 
    {
        fprintf(stderr, "Fork failed.\n");
        return 1;
    }

    if (pid > 0) 
    {
        signal(SIGUSR1, graceful_exit_handler);
        signal(SIGINT, sigint_handler);
  
        while (true) 
        {
            printMenu();
            scanf("%d", &number);

            if (number < 1 || number > 7)
            {
                printf("Invalid choice, please try again.\n");
                continue;
            }
            parentBytesCounter = write(pipeToChild[1], &number, sizeof(number)); // Write the number to the pipe
            while (parentBytesCounter != sizeof(number))
            {
                parentBytesCounter += write(pipeToChild[1], &number, sizeof(number));
            }

            if (number >= 1 && number <= 3)
            {
                int inputArrSize = 0, outputArrSize = 0;
                char** output = NULL, **input = NULL;

                input = readInput(&inputArrSize, number);
                parentBytesCounter = write(pipeToChild[1], &inputArrSize, sizeof(int));
                while (parentBytesCounter != sizeof(int))
                {
                    parentBytesCounter += write(pipeToChild[1], &number, sizeof(number));
                }

                ReadOrWriteToPipe(input, inputArrSize, pipeToChild, WRITE);

                parentBytesCounter = read(pipeToParent[0], &outputArrSize, sizeof(int));
                while (parentBytesCounter != sizeof(int))
                {
                    parentBytesCounter += read(pipeToParent[0], &outputArrSize, sizeof(int));
                }

                output = (char**)malloc(sizeof(char*) * outputArrSize);
                checkAllocation(output);
                ReadOrWriteToPipe(output, outputArrSize, pipeToParent, READ);
                
                if (outputArrSize == 0)
                {
                    printf("Aircraft was not found in our DB.\n");
                }

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

            else if (number == 4)
            {
                parentBytesCounter = read(pipeToParent[0], &result, sizeof(result));
                while (parentBytesCounter != sizeof(result))
                {
                    parentBytesCounter += read(pipeToParent[0], &result, sizeof(result));
                }

                printf("Successfully updated the DB files.\n");
            }

            else if (number == 5)
            {
                parentBytesCounter = read(pipeToParent[0], &result, sizeof(result));
                while (parentBytesCounter != sizeof(result))
                {
                    parentBytesCounter += read(pipeToParent[0], &result, sizeof(result));
                }

                if (result == FINISH)
                {
                    printf("Successfully zipped the DB files.\n");
                }

                else
                {
                    printf("Zip Failed.\n");
                }               
            }

            else if (number == 6)
            {
                parentBytesCounter = read(pipeToParent[0], &childPID, sizeof(pid_t));
                while (parentBytesCounter != sizeof(pid_t))
                {
                    parentBytesCounter += read(pipeToParent[0], &childPID, sizeof(pid_t));
                }

                printf("Child process PID: %d\n", childPID);
            }

            else if (number == 7)
            {
                int exitCode;
                parentBytesCounter = read(pipeToParent[0], &childPID, sizeof(pid_t));
                while (parentBytesCounter != sizeof(pid_t))
                {
                    parentBytesCounter += read(pipeToParent[0], &childPID, sizeof(pid_t));
                }

                parentBytesCounter = read(pipeToParent[0], &exitCode, sizeof(int));
                while (parentBytesCounter != sizeof(int))
                {
                    parentBytesCounter += read(pipeToParent[0], &exitCode, sizeof(int));
                }


                kill(childPID, SIGUSR1);
                printf("Gracefully exiting.\nChild process's exit code: %d\n", exitCode);
                wait(NULL);
                close(pipeToChild[WRITE]);
                close(pipeToParent[READ]);
                exit(EXIT_SUCCESS);
            }

            printf("\n");
        }
    } 
    
    else if (pid == 0) //Child process.
    {
        signal(SIGUSR1, graceful_exit_handler);
        signal(SIGINT, sigint_handler);

        if (firstChildIteration)
        {
            prepareToUnzip();
            char** dirList = NULL;
            int listSize = 0;
            dirList = createDirList(&listSize);
            dataBase = getDataBase(listSize, dirList);

            for (int i = 0; i < listSize; i++)
            {
                free(dirList[i]);
            }
            
            if (listSize > 0)
                free(dirList);

            firstChildIteration = false;
        }

        while (true)
        {
            childBytesCounter = read(pipeToChild[0], &number, sizeof(number));
            while (childBytesCounter != sizeof(number))
            {
                childBytesCounter += read(pipeToChild[0], &number, sizeof(number));
            }

            child_process(pipeToChild, pipeToParent, number, &dataBase);
        }

        exit(0);
    }

    return 0;
}