#include "Utilities.h"

int main ()
{
    //the main process interacts with the fifo pipe founded in 
    // .../tmp/flight_fifo
    const char* myfifo = "../../child_process/tmp/filghts_fifo";
    sem_t* DBServiceSema = sem_open(READER_SEMAPHORE, 0);
    sem_t* flightServiceSema = sem_open(WRITER_SEMAPHORE, 0);
    if (DBServiceSema == SEM_FAILED || flightServiceSema == SEM_FAILED)
    {
        perror("Semaphore failed.\n Exiting program...\n");
        exit(-1);
    }
    int fd, BytesCounter = 0, result = 0, Opcode = 0;

    if ((fd = open(myfifo, O_RDWR)) == -1)
    {
        printf("Error code %d\n", errno);
        perror("Failed to open FIFO.");
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

            ReadOrWriteToPipe(input, inputArrSize, fd, WRITE);
            sem_post(DBServiceSema);
            sem_wait(flightServiceSema);
            
            if (Opcode >= 2 && Opcode <= 4)
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
            sem_post(DBServiceSema);
            sem_wait(flightServiceSema);
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
            sem_post(DBServiceSema);
            sem_wait(flightServiceSema);
            int exitCode;
            BytesCounter = read(fd, &exitCode, sizeof(int));
            while (BytesCounter != sizeof(int))
            {
                BytesCounter += read(fd, &exitCode, sizeof(int));
            }
            printf("Gracefully exiting.\nDBService exit code: %d\n", exitCode);
            close(fd);
            sem_close(DBServiceSema);
            sem_close(flightServiceSema);
            exit(EXIT_SUCCESS);
        }

        printf("\n");
    }
    return 0;
}