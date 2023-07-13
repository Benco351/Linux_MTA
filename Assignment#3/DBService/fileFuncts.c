#include "Utilities.h"

//Counts file's num of rows.
int checkRows(FILE* file)
{
    int counter = 0, debug = 0;
    char input, line[MAX_SIZE];

    while (fgets(line, sizeof(line), file) != NULL) 
    {
        counter++;
    }

    fseek(file, 0, SEEK_SET);
    return (counter - 1);
}

//Opens an airport's database.
void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile)
{
    char dir1[MAX_SIZE] = "../flightsDB/";
    char dir2[MAX_SIZE];
    strcat(dir1, airportName);
    strcat(dir1, "/");
    strcat(dir1, airportName);
    strcpy(dir2, dir1);

    strcat(dir1, ".dpt");
    strcat(dir2, ".arv");

    *departureFile = fopen(dir1, "r");
    *arrivalFile = fopen(dir2, "r");
}