#include "Q3.h"

int main (int argc, char* argv[])
{
    FILE* arrivalFile, *departureFile;
    int sizeOfDirList = 0;
    char** dirList = createDirList(&sizeOfDirList);

    for (int i = 0; i < sizeOfDirList; i++)
    {
        openFilesByAirportName(dirList[i], &departureFile, &arrivalFile);
        findAirCrafts(departureFile, argv, argc - 1);
        findAirCrafts(arrivalFile, argv, argc - 1);

        fclose(arrivalFile);
        fclose(departureFile);
    }

    for (int i = 0; i < sizeOfDirList; i++)
    {
        free(dirList[i]);
    }

    free(dirList);

    return 1;
}