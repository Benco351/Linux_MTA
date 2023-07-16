#include "Utilities.h"

int main()
{
    const char* myfifo = "/shared-volume/filghts_fifo";
    sem_t* DBServiceSema = sem_open(DB_SERVICE_SEMAPHORE, 0);
    sem_t* flightServiceSema = sem_open(FLIGHTS_SERVICE_SEMAPHORE, 0);
    if (DBServiceSema == SEM_FAILED || flightServiceSema == SEM_FAILED)
    {
        perror("Semaphore failed.\n Error");
        exit(-1);
    }
    int FIFO, BytesCounter = 0, result = 0, Opcode = 0;

    if ((FIFO = open(myfifo, O_RDWR)) == -1)
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

        BytesCounter = write(FIFO, &Opcode, sizeof(Opcode)); // Write the Opcode to the pipe
        while (BytesCounter != sizeof(Opcode))
        {
            BytesCounter += write(FIFO, &Opcode, sizeof(Opcode));
        }

        if (Opcode >= FETCH_DATA && Opcode <= AIRCRAFT_SCHEDULE)
        {
            int inputArrSize = 0, outputArrSize = 0;
            char** output = NULL, **input = NULL;

            input = readInput(&inputArrSize, Opcode);
            BytesCounter = write(FIFO, &inputArrSize, sizeof(int));
            while (BytesCounter != sizeof(int))
            {
                BytesCounter += write(FIFO, &Opcode, sizeof(Opcode));
            }

            Opcode == FETCH_DATA ? 
            printf("Fetching airports data, please hold.\n"):
            printf("Fetching data from DB, please hold.\n");

            ReadOrWriteToPipe(input, inputArrSize, FIFO, WRITE);
            sem_post(DBServiceSema);
            sem_wait(flightServiceSema);

            BytesCounter = read(FIFO, &result, sizeof(int));
            while (BytesCounter != sizeof(int))
            {
                BytesCounter += read(FIFO, &result, sizeof(int));
            }

            if (result == EMPTY_INPUT)
            {
                printf("No input inserted. Please try again...\n");
            }

            else if (result == SCRIPT_FAILURE)
            {
                printf("Error running script. Please try again.\n");
            }
            
            else if (Opcode >= INCOMING_FLIGHTS && Opcode <= AIRCRAFT_SCHEDULE)
            {
                outputArrSize = result;
                output = (char**)malloc(sizeof(char*) * outputArrSize);
                checkAllocation(output);
                ReadOrWriteToPipe(output, outputArrSize, FIFO, READ);

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

            else if (Opcode == FETCH_DATA && result == FINISH)
            {
                printf("Successfully updated the DB files.\n");
            }
        }

        else if (Opcode == ZIP_DB)
        {
            sem_post(DBServiceSema);
            sem_wait(flightServiceSema);
            BytesCounter = read(FIFO, &result, sizeof(result));
            while (BytesCounter != sizeof(result))
            {
                BytesCounter += read(FIFO, &result, sizeof(result));
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

        else if (Opcode == SHUTDOWN)
        {
            sem_post(DBServiceSema);
            printf("Shutting down....\n");
            close(FIFO);
            sem_close(DBServiceSema);
            sem_close(flightServiceSema);
            exit(EXIT_SUCCESS);
        }

        printf("\n");
    }
    return 0;
}