#include "Q3.h"

void main (int argc, char* argv[])
{
    FILE* arrivalFile, *departureFile;
    int sizeOfDirList = 0;
    char** dirList = createDirList(&sizeOfDirList);
    FlightData* data;

    for (int i = 1; i < sizeOfDirList; i++)
    {
        openFilesByAirportName(dirList[i], &departureFile, &arrivalFile);
        findAirCrafts(departureFile, argv, argc - 1);
        findAirCrafts(arrivalFile, argv, argc - 1);
    }
}

//Count dir list
//Create dir list
//For each item, open file
//For each file, call func