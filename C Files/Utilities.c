#include "Utilities.h"

FlightData splitS(char* str)
{
    FlightData FD;

    strcpy(FD.icao24, strtok(str, ","));
    strcpy(FD.firstSeen, strtok(NULL, ","));
    strcpy(FD.departureAirPort, strtok(NULL, ","));
    strcpy(FD.lastSeen, strtok(NULL, ","));
    strcpy(FD.arrivalAirPort, strtok(NULL, ","));
    strcpy(FD.flightNumber, strtok(NULL, ",\n"));

    return FD;
}

// char** get_flights_data_in_airport(char* fileName)
// {
//     char** flightsData = NULL;
//     FILE* newFile = fopen(fileName, "r");
//     checkAllocation(newFile);

//     int numOfFlights = howManyRowsInFile(newFile);
//     flightsData = (char**)malloc(sizeof(char*) * numOfFlights);
//     checkAllocation(flightsData);
//     char temp[MAX_SIZE];
//     fgets(temp, MAX_SIZE, newFile); // get rid of info line
//     int rowLen = 0;

//     for (int i = 0; i < numOfFlights; i++)
//     {
//         flightsData[i] = (char*)malloc(sizeof(char) * MAX_SIZE);
//         checkAllocation(flightsData[i]);
//         fgets(temp, MAX_SIZE, newFile);
//         char** processedData = splitString(temp);
//         arrangeString(&flightsData[i], processedData);
//         rowLen = strlen(flightsData[i]);
//         flightsData[i] = (char*)realloc(flightsData[i], rowLen + 1);
//         flightsData[rowLen] = '\0';
//         free(processedData);
//     }

//     fclose(newFile);
//     return flightsData;
// }

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

// char** splitString(char* str)
// {
//     char** output = (char**)malloc(sizeof(char*) * NUM_OF_COLUMNS);
//     checkAllocation(output);
//     int size = 0;

//     for (int i = 0; i < NUM_OF_COLUMNS; i++)
//     {
//         output[i] = (char*)malloc(sizeof(char) * MAX_SIZE);
//         checkAllocation(output[i]);

//         if (i == 0)
//         {
//             strcpy(output[i], strtok(str, ","));
//         }

//         else if (i > 0)
//         {
//             strcpy(output[i], strtok(NULL, ","));
//         }

//         size = strlen(output[i]) + 1;

//         if (output[i][size - 2] == '\n')
//             size -= 1;

//         output[i] = realloc(output[i], size);
//         checkAllocation(output[i]);
//         output[size] = '\0';
//     }

//     return output;
// }

// void arrangeString(char** emptyString, char** dataString)
// {
//     strcat(*emptyString, "Flight #");
//     strcat(*emptyString, dataString[FLIGHT_NUMBER]);
//     strcat(*emptyString, " arriving from ");
//     strcat(*emptyString, dataString[DEPARTURE_AIRPORT]);
//     strcat(*emptyString, ", takeoff at ");
//     strcat(*emptyString, dataString[FIRST_SEEN]);
//     strcat(*emptyString, " landed at ");
//     strcat(*emptyString, dataString[LAST_SEEN]);
//     strcat(*emptyString, "\0");
// }

void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile) //Opens an airport's database.
{
    char dir1[MAX_SIZE] = "flightsDB/";
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

void loadDatabase(int numOfArgs, char* airports[]) //Loads database according to arguments.
{
    char command[MAX_SIZE] = "/bin/bash /../Bash/flightScanner.sh";
    for (int i = 1; i < numOfArgs; i++)
    {
        strcat(command, " $");
        strcat(command, airports[i]);
    }

    system(command);
}

//this function gets and open file of specific airport, the aircrafts searched for and their number
//returns all flights for said aircrafts in given airport

char* unix_time_to_date(const char* timestamp_str) {
    time_t timestamp = atoi(timestamp_str);
    struct tm* timeinfo_local;
    struct tm* timeinfo_target;
    char* buffer = (char*)malloc(sizeof(char)*80);

    // Convert Unix timestamp to struct tm in local timezone
    timeinfo_local = localtime(&timestamp);
    if (timeinfo_local == NULL) {
        fprintf(stderr, "localtime_r failed\n");
        return NULL;
    }

    // Add timezone offset to get GMT+3 time
    timeinfo_local->tm_hour += 3;
    mktime(timeinfo_local);

    // Convert struct tm to GMT+3 timezone
    time_t timestamp_target = mktime(timeinfo_local);
    timeinfo_target = gmtime(&timestamp_target);

    // Format date string
    if (strftime(buffer, 80, "%Y-%m-%d %H:%S:%M", timeinfo_target) == 0) {
        fprintf(stderr, "strftime failed\n");
        return NULL;
    }

    return buffer;
}

char** createDirList(int* size)
{
    char** output = NULL;
    int phySize = 1, logSize = 0;

    output = (char**)malloc(sizeof(char*) * phySize);
    checkAllocation;

    DIR* directory = opendir("../flightsDB/");
    checkAllocation(directory);

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) 
    {
        // Exclude hidden files/directories that start with a dot
        if (entry->d_name[0] != '.') 
        {
            if (logSize == phySize)
            {
                phySize *= 2;
                output = (char**)realloc(output, phySize);
                checkAllocation(output);
            }

            output[logSize] = (char*)malloc(sizeof(char) * (NAME_LEN + 1));
            checkAllocation(output[logSize]);
            strcpy(output[logSize], entry->d_name);
            output[logSize][NAME_LEN] = '\0';

            logSize++;
        }
    }

    output = (char**)realloc(output, logSize);
    checkAllocation(output);
    *size = logSize;

    closedir(directory);
    return output;
}