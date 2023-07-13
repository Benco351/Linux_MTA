#include "Utilities.h"

//Prints the main menu.
void printMenu()
{
    printf("1. Fetch airports data\n");
    printf("2. Print airports incoming flights\n");
    printf("3. Print airports full flight schedule\n");
    printf("4. Print aircraft full flight schedule\n");
    printf("5. Zip DB files\n");
    printf("6. Shutdown\n");
    printf("Please make your choice <1,2,3,4,5,6>: ");
}

//Reads/writes from/to the FIFO.
void ReadOrWriteToPipe(char** output, int O_size, int FIFO, bool SIG)
{
    int currentStrSize = 0, bytesCounter = 0;

    if (SIG == READ)
    {
        for (int i = 0; i < O_size; i++)
        {
            bytesCounter = read(FIFO, &currentStrSize, sizeof(currentStrSize));
            while (bytesCounter != sizeof(int))
            {
                bytesCounter += read(FIFO, &currentStrSize, sizeof(currentStrSize));
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

//Checks whether input is valid.
bool isValidChar(char input)
{
    return (input == ',' || input == '\n'|| (input >= 65 && input <= 122) || (input >= 48 && input <= 57));
}

//Reads the input from user.
char** readInput(int* size, int choice)
{
    int arrLogSize = 0, arrPhySize = 0, currentStringLogSize = 0, currentStringPhySize = 0;
    char** output = NULL;

    printf("Please enter desired input seperated by a single comma (,).\n"
           "For example: LLBG,LLTK\n\n");

    char input = (char)getchar();

    if (input == '\n') //case of previous buffer.
    {
        input = (char)getchar();
    }

    while (input != '\n')
    {
        while (!isValidChar(input))
        {
            input = (char)getchar();
        }

        if (input >= 97 && choice != 4)
        {
            input -= 32;
        }

        if (input == ',' || input == '\n')
        {
            if (currentStringLogSize == currentStringPhySize)
            {
                currentStringPhySize += 1;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }

            else if (currentStringLogSize + 1 < currentStringPhySize)
            {
                currentStringPhySize = currentStringLogSize + 1;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }

            output[arrLogSize][currentStringLogSize] = '\0';
            arrLogSize++;
            currentStringLogSize = 0;
            currentStringPhySize = 0;

            if (input == ',')
            {
                input = (char)getchar();
            }
            continue;
        }

        if (arrLogSize == arrPhySize)
        {
            if (arrPhySize == 0)
            {
                arrPhySize += 1;
                output = (char**)malloc(sizeof(char*));
            }

            else
            {
                arrPhySize *= 2;
                output = (char**)realloc(output, arrPhySize * sizeof(char*));
            }

            checkAllocation(output);
        }

        if (currentStringLogSize == currentStringPhySize)
        {
            if (currentStringPhySize == 0)
            {
                currentStringPhySize += 4;
                output[arrLogSize] = (char*)malloc(sizeof(char) * currentStringPhySize);
                checkAllocation(output[arrLogSize]);
            }

            else if (currentStringPhySize > 0)
            {
                currentStringPhySize *= 2;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }
        }

        output[arrLogSize][currentStringLogSize] = input;
        currentStringLogSize++;
        input = (char)getchar();

        if (input == ',' || input == '\n')
        {
            if (currentStringLogSize == currentStringPhySize)
            {
                currentStringPhySize += 1;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }

            else if (currentStringLogSize + 1 < currentStringPhySize)
            {
                currentStringPhySize = currentStringLogSize + 1;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }

            output[arrLogSize][currentStringLogSize] = '\0';
            arrLogSize++;
            currentStringLogSize = 0;
            currentStringPhySize = 0;

            if (input == ',')
            {
                input = (char)getchar();
            }
            continue;
        }
    }

    if (arrPhySize > arrLogSize)
    {
        output = (char**)realloc(output, arrLogSize * sizeof(char*));
        checkAllocation(output);
    }

    *size = arrLogSize;
    return output;
}

//Verifies memory allocation.
void checkAllocation(void* pointer)
{
    if (pointer == NULL)
    {
        printf("Memory allocation failed. Exiting program...\n");
        exit(-1);
    }
}