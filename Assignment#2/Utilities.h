#ifndef UTILLITIES_H
#define UTILLITIES_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

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

typedef struct airports
{
    int SizeArivals;
    FlightData* arivals;
    int SizeDeparturs;
    FlightData* Departurs;
} AirPorts;

typedef struct DataBase
{
    int nofAirports;
    AirPorts* airPortsArr;
    char** airPortsNames;
} DB;

//////////////////////////////Data Base Functions//////////////////////////////
void freeDataBase(DB* db);
DB* getDataBase(int numOfArgs, char* airports[]);
void swap(char* arr[], int i, int j);
int partition(char* arr[], int low, int high);
void quickSort(char* arr[], int low, int high);
char** reorderStringArray(int numOfArgs, char* airports[]);
int checkRows(FILE* file);
//////////////////////////////General Functions//////////////////////////////
void graceful_exit_handler(int signum);
void sigint_handler(int signum);
void child_process(int pipeToChild[2], int pipeToParent[2], int number, DB* dataBase);
void printMenu();
int quickSearch(char* arr[], int size, char* target);
char* unix_time_to_date(const char* timestamp_str);
int howManyRowsInFile(FILE* file);
void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile);
void loadDatabase(int numOfArgs, char* airports[]);
FlightData splitS(char* str);
char** createDirList(int* size);
void checkAllocation(void* ptr);
bool isValidChar(char input);
char** readInput(int* arrSize);
//////////////////////////////Q1 Functions//////////////////////////////
char** printFlightsToAirport(char* airportName, DB db, int pipeToParent[2], int* logSize);
char* printFlightsData(FlightData object);
void runQ1(char* parameters[], int numOfParameters, DB db, int pipeToParent[2]);
//////////////////////////////Q2 Functions//////////////////////////////
void printAirportSchedule(char* airportName, DB db, int pipeToParent[2]);
int compareFlights(const void* a, const void* b);
void printFullSchedule(FlightData object);
void runQ2(char* parameters[], int numOfParameters, DB db, int pipeToParent[2]);
//////////////////////////////Q3 Functions//////////////////////////////
void findAirCrafts(char** aircrafts, int nofAirCrafts, DB db, int pipeToParent[2]);
void printQ3(FlightData FD);
void runQ3(char* parameters[], int numOfParameters, DB db, int pipeToParent[2]);
//////////////////////////////Q4 Functions//////////////////////////////
void runQ4();
//////////////////////////////Q5 Functions//////////////////////////////
//////////////////////////////Q6 Functions//////////////////////////////
//////////////////////////////Q7 Functions//////////////////////////////

#endif