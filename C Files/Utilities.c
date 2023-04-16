#include "Utilities.h"
#define MAX_SIZE 300
#define NUM_OF_COLUMNS 6
#define ICAO24 0
#define FIRST_SEEN 1
#define DEPARTURE_AIRPORT 2
#define LAST_SEEN 3
#define ARRIVAL_AIRPORT 4
#define FLIGHT_NUMBER 5

char** get_flights_data_in_airport(char* fileName)
{
    char** flightsData = NULL;
    FILE* newFile = fopen(fileName, "r");
    checkAllocation(newFile);

    int numOfFlights = howManyRowsInFile(newFile);
    flightsData = (char**)malloc(sizeof(char*) * numOfFlights);
    checkAllocation(flightsData);
    char temp[MAX_SIZE];
    fgets(temp, MAX_SIZE, newFile); // get rid of info line
    int rowLen = 0;

    for (int i = 0; i < numOfFlights; i++)
    {
        flightsData[i] = (char*)malloc(sizeof(char) * MAX_SIZE);
        checkAllocation(flightsData[i]);
        fgets(temp, MAX_SIZE, newFile);
        char** processedData = splitString(temp);
        arrangeString(&flightsData[i], processedData);
        rowLen = strlen(flightsData[i]);
        flightsData[i] = (char*)realloc(flightsData[i], rowLen + 1);
        flightsData[rowLen] = '\0';
        free(processedData);
    }

    fclose(newFile);
    return flightsData;
}

int howManyRowsInFile(FILE* fileName)
{
    int counter = 0;
    char input;

    for (input = getc(fileName); input != EOF; input = getc(fileName))
    {
        if (input == '\n')
            counter++;
    }

    fseek(fileName, 0, SEEK_SET);

    return counter - 1;
}

void checkAllocation(void* pointer)
{
    if (pointer == NULL)
        exit(-1);
}

char** splitString(char* str)
{
    char** output = (char**)malloc(sizeof(char*) * NUM_OF_COLUMNS);
    checkAllocation(output);
    int size = 0;

    for (int i = 0; i < NUM_OF_COLUMNS; i++)
    {
        output[i] = (char*)malloc(sizeof(char) * MAX_SIZE);
        checkAllocation(output[i]);

        if (i == 0)
        {
            strcpy(output[i], strtok(str, ","));
        }

        else if (i > 0)
        {
            strcpy(output[i], strtok(NULL, ","));
        }

        size = strlen(output[i]) + 1;

        if (output[i][size - 2] == '\n')
            size -= 1;

        output[i] = realloc(output[i], size);
        checkAllocation(output[i]);
        output[size] = '\0';
    }

    return output;
}

void arrangeString(char** emptyString, char** dataString)
{
    strcat(*emptyString, "Flight #");
    strcat(*emptyString, dataString[FLIGHT_NUMBER]);
    strcat(*emptyString, " arriving from ");
    strcat(*emptyString, dataString[DEPARTURE_AIRPORT]);
    strcat(*emptyString, ", takeoff at ");
    strcat(*emptyString, dataString[FIRST_SEEN]);
    strcat(*emptyString, " landed at ");
    strcat(*emptyString, dataString[LAST_SEEN]);
    strcat(*emptyString, "\0");
}