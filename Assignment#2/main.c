#include "Utilities.h"

int main() 
{
    int pipeToChild[2]; // Pipe for sending numbers from parent to child
    int pipeToParent[2]; // Pipe for sending signals from child to parent
    int number;
    pid_t pid, childPID;
    DB* dataBase;
    bool firstChildIteration = true;

    if (pipe(pipeToChild) == -1 || pipe(pipeToParent) == -1) {
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

            write(pipeToChild[1], &number, sizeof(number)); // Write the number to the pipe

            if (number >= 1 && number <= 3)
            {
                int inputArrSize = 0, outputArrSize = 0;
                char** output = NULL, **input = NULL;
                input = readInput(&inputArrSize, number);
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

            else if (number == 4)
            {
                printf("Successfully updated the DB.\n");
            }

            else if (number == 5)
            {
                int result = 0;
                read(pipeToParent[0],&result,sizeof(result));
                if(result == 0)
                {
                    printf("Successfully zipped the DB files.\n");
                }
                else
                {
                    printf("Zip Failed\n");
                }               
            }

            else if (number == 6)
            {
                read(pipeToParent[0], &childPID, sizeof(pid_t));
                printf("Child process PID: %d\n", childPID);
            }

            else if (number == 7)
            {
                int result = 0;
                read(pipeToParent[0],&result,sizeof(result));
                if(result == 0)
                {
                    printf("Successfully zipped the DB files.\n");
                }
                else
                {
                    printf("Zip Failed\n");
                }          
                int exitCode;
                read(pipeToParent[0], &childPID, sizeof(pid_t));
                read(pipeToParent[0], &exitCode, sizeof(int));
                kill(childPID, SIGUSR1);
                wait(NULL);
                printf("Gracefully exiting.\nChild process's exit code: %d\n", exitCode);
                exit(EXIT_SUCCESS);
            }

            printf("\n");
        }
    } 
    
    else if (pid == 0) 
    {
        // Child process
        if (firstChildIteration)
        {
        int result  = 0;
            char cwd[PATH_MAX];
            char tmp[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            int size = strlen(cwd);
            cwd[size - 6] ='\0';
            strcpy(tmp,cwd);
            strcat(tmp,"/flightsDB.zip");
            strcat(cwd,"/flightsDB");
            bool DBFolderExists= doesDBFolderExists();
            if(!DBFolderExists)
            {
                mkdir(cwd,0777);
            }
            bool zipExists = doesZipExists();
            if (zipExists)
            {  
                if (cwd != NULL) {
                    result = unzipFolder(tmp,cwd);
                }
            }
            char** dirList = NULL;
            int listSize = 0;
            dirList= createDirList(&listSize);
            dataBase = getDataBase(listSize,dirList);
            firstChildIteration = false;
        }

        while (true)
        {
            read(pipeToChild[0], &number, sizeof(number));
            child_process(pipeToChild, pipeToParent, number, dataBase);
        }

        exit(0);
    }

    return 0;
}