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
        char firstSeen[FD_MAX + 5];
        char departureAirPort[FD_MAX];
        char lastSeen[FD_MAX + 5];
        char arrivalAirPort[FD_MAX];
        char flightNumber[FD_MAX];
        unsigned short arrivalOrDeparture;
    } FlightData;

    //////////////////////////////General Functions//////////////////////////////
    char* unix_time_to_date(const char* timestamp_str);
    int howManyRowsInFile(FILE* file);
    void checkAllocation(void* pointer);
    void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile);
    void loadDatabase(int numOfArgs, char* airports[]);
    FlightData splitS(char* str);
    char** createDirList(int* size);
    //////////////////////////////Q1 Functions//////////////////////////////
    void printFlightsToAirport(char* airportName);
    void printFlightsData(FlightData object);
    void runQ1(char* parameters[], int numOfParameters);
    //////////////////////////////Q2 Functions//////////////////////////////
    void printAirportSchedule(char* airportName);
    int compareFlights(const void* a, const void* b);
    void printFullSchedule(FlightData object);
    void runQ2(char* parameters[], int numOfParameters);
    //////////////////////////////Q3 Functions//////////////////////////////
    void findAirCrafts(FILE* f, char** aircrafts, int nofAirCrafts);
    void printQ3(FlightData FD);
    void runQ3(char* parameters[], int numOfParameters);
    //////////////////////////////Q4 Functions//////////////////////////////
    void runQ4();
    //////////////////////////////Q5 Functions//////////////////////////////
    //////////////////////////////Q6 Functions//////////////////////////////
    //////////////////////////////Q7 Functions//////////////////////////////

#endif