
#ifndef UTILLITIES_H
#define UTILLITIES_H
    #define _CRT_SECURE_NO_WARNINGS
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <stddef.h>
    #include <time.h>
    #include <dirent.h>

    #define MAX_SIZE 300
    #define FD_MAX 15
    #define ARRIVAL 0
    #define DEPARTURE 1
    #define NAME_LEN 4

    typedef struct flightData
    {
        char icao24[FD_MAX];
        char firstSeen[FD_MAX];
        char departureAirPort[FD_MAX];
        char lastSeen[FD_MAX];
        char arrivalAirPort[FD_MAX];
        char flightNumber[FD_MAX];
        unsigned short arrivalOrDeparture;
    } FlightData;

    // char** get_flights_data_in_airport(char* fileName);
    int howManyRowsInFile(FILE* file);
    void checkAllocation(void* pointer);
    // char** splitString(char* str);
    // void arrangeString(char** emptyString, char** dataString);
    void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile);
    void loadDatabase(int numOfArgs, char* airports[]);
    FlightData splitS(char* str);
    // char** findAirCrafts(FILE* f, char** aircrafts, int nofAirCrafts);
    // char* arrangeAirCraftString(FlightData FD);
    char** createDirList(int* size);
#endif
