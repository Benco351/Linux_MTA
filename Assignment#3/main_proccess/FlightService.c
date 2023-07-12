#include "Utillities.h"



int main ()
{
    //the main process interacts with the fifo pipe founded in 
    // .../tmp/flight_fifo
    const char* myfifo ="./tmp/filghts_fifo";
    int fd,BytesCounter =0,result = 0,Opcode = 0;

   if(fd = open(myfifo,O_RDWR) == -1)
    {
        printf("Error Opcode %d\n", errno);
        perror("Failed to open fifo");
    }

    
    while (true) 
    {
        printMenu();
        scanf("%d", &Opcode);

    
            if (Opcode < 1 || Opcode > 6)
            {
                printf("Invalid choice, please try again.\n");
                continue;
            }
            BytesCounter = write(fd, &Opcode, sizeof(Opcode)); // Write the Opcode to the pipe
            while (BytesCounter != sizeof(Opcode))
            {
                BytesCounter += write(fd, &Opcode, sizeof(Opcode));
            }

            if (Opcode >= 1 && Opcode <= 4)
            {
                int inputArrSize = 0, outputArrSize = 0;
                char** output = NULL, **input = NULL;

                input = readInput(&inputArrSize, Opcode);
                BytesCounter = write(fd, &inputArrSize, sizeof(int));
                while (BytesCounter != sizeof(int))
                {
                    BytesCounter += write(fd, &Opcode, sizeof(Opcode));
                }

                ReadOrWriteToPipe(input, inputArrSize,fd, WRITE);
                
                if(Opcode >=2 && Opcode <=4)
                {

                    BytesCounter = read(fd, &outputArrSize, sizeof(int));
                    while (BytesCounter != sizeof(int))
                    {
                        BytesCounter += read(fd, &outputArrSize, sizeof(int));
                    }

                    output = (char**)malloc(sizeof(char*) * outputArrSize);
                    checkAllocation(output);
                    ReadOrWriteToPipe(output, outputArrSize, fd, READ);
                    
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
                else if (Opcode == 1)
                {
                    BytesCounter = read(fd, &result, sizeof(result));
                    while (BytesCounter != sizeof(result))
                    {
                        BytesCounter += read(fd, &result, sizeof(result));
                    }

                    printf("Successfully updated the DB files.\n");
                }

            }


            else if (Opcode == 5)
            {
                BytesCounter = read(fd, &result, sizeof(result));
                while (BytesCounter != sizeof(result))
                {
                    BytesCounter += read(fd, &result, sizeof(result));
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

            else if (Opcode == 6)
            {
                int exitCode;
                BytesCounter = read(fd, &exitCode, sizeof(int));
                while (BytesCounter != sizeof(int))
                {
                    BytesCounter += read(fd, &exitCode, sizeof(int));
                }
                printf("Gracefully exiting.\n DbService exit code: %d\n", exitCode);
                close(fd);
                exit(EXIT_SUCCESS);
            }

            printf("\n");
    }
    
    






    return 0;
}