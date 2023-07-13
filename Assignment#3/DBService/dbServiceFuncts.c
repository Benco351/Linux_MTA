#include "Utilities.h"

//DB service main function.
void dbServiceProcess(int FIFO, int Opcode, DB** dataBase)
{
    int arrSize = 0, exitCode, result = 0, bytesCounter = 0;
    char** output = NULL;
    DB* newDB = NULL;

    if (Opcode >= INCOMING_FLIGHTS && Opcode <= AIRCRAFT_SCHEDULE)
    {
        bytesCounter = read(FIFO, &arrSize, sizeof(int));
        while (bytesCounter != sizeof(int))
        {
            bytesCounter += read(FIFO, &arrSize, sizeof(int));
        }

        output = (char**)malloc(sizeof(char*) * arrSize);
        checkAllocation(output);
        
        ReadOrWriteToPipe(output, arrSize, FIFO, READ);
        runRequestOnDB(output, arrSize, *dataBase, FIFO, Opcode);

        for (int i = 0; i < arrSize; i++)
        {
            free(output[i]);
        }

        if (arrSize > 0)
            free(output);
    }

    switch (Opcode)
    {
        case FETCH_DATA:
            newDB = reRunScript(*dataBase, FIFO);
            freeDataBase(*dataBase);
            *dataBase = newDB;
            result = FINISH;
            bytesCounter = write(FIFO, &result, sizeof(result));
            while (bytesCounter != sizeof(result))
            {
                bytesCounter += write(FIFO, &result, sizeof(result));
            }
            break;
        case ZIP_DB:
            prepareToZip();
            result = FINISH;
            bytesCounter = write(FIFO, &result, sizeof(result));
            while (bytesCounter != sizeof(result))
            {
                bytesCounter += write(FIFO, &result, sizeof(result));
            }
            break;
        case SHUTDOWN:
            freeDataBase(*dataBase);
            close(FIFO);
            break;
        default:
            break;
    }
}

//Reads/writes from/to the FIFO.
void ReadOrWriteToPipe(char** output, int O_size, int FIFO, bool SIG)
{
    int currentStrSize = 0, bytesCounter = 0;

    if (SIG == READ)
    {
        for (int i = 0; i < O_size; i++)
        {
            bytesCounter = read(FIFO, &currentStrSize, sizeof(int));
            while (bytesCounter != sizeof(int))
            {
                bytesCounter += read(FIFO, &currentStrSize, sizeof(int));
            }

            output[i] = (char*)malloc(sizeof(char) * (currentStrSize + 1));
            checkAllocation(output[i]);

            bytesCounter = read(FIFO, output[i], currentStrSize);
            while (bytesCounter != currentStrSize)
            {
                bytesCounter += read(FIFO, output[i], currentStrSize);
            }
            output[i][currentStrSize] = '\0';
        }
    }

    else
    {
        for (int i = 0; i < O_size; i++)
        {
            currentStrSize = (int)strlen(output[i]);
            bytesCounter = write(FIFO, &currentStrSize, sizeof(currentStrSize));
            while (bytesCounter != sizeof(currentStrSize))
            {
                bytesCounter += write(FIFO, &currentStrSize, sizeof(currentStrSize));
            }

            bytesCounter = write(FIFO, output[i], currentStrSize);
            while (bytesCounter != currentStrSize)
            {
                bytesCounter += write(FIFO, output[i], currentStrSize);
            }
            output[i][currentStrSize] = '\0';
        }
    }
}