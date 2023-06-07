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
#define READ 0
#define WRITE 1
#define DB_EXISTS 1
#define DB_DOES_NOT_EXISTS 0
#define GENERATE_DB 8
#define MISSION1 1
#define MISSION2 2
#define MISSION3 3

typedef struct flightData
{
    char icao24[FD_MAX];
    char firstSeen[FD_MAX + 10];
    char departureAirPort[FD_MAX];
    char lastSeen[FD_MAX + 10];
    char arrivalAirPort[FD_MAX];
    char flightNumber[FD_MAX];
    unsigned short arrivalOrDeparture;
} FlightData;

typedef struct airports
{
    int SizeArrivals;
    FlightData* arrivals;
    int SizeDepartures;
    FlightData* Departures;
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
char** reorderStringArray(int numOfArgs, char* airports[]);
char* unix_time_to_date(const char* timestamp_str);
void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile);
//////////////////////////////General Functions//////////////////////////////
int checkRows(FILE* file);
bool doesZipExists();
int compareStrings(const void* a, const void* b);
void ReadOrWriteToPipe(char** output, int O_size, int pipe[2], bool SIG);
int quickSearch(char* arr[], int size, char* target);
void checkAllocation(void* ptr);
FlightData splitS(char* str);
char** createDirList(int* size);
//////////////////////////////Parent Functions//////////////////////////////
void graceful_exit_handler(int signum);
void sigint_handler(int signum);
void printMenu();
bool isValidChar(char input);
char** readInput(int* size, int choice);
//////////////////////////////Child Functions//////////////////////////////
void child_process(int pipeToChild[2], int pipeToParent[2], int number, DB* dataBase);
void loadDatabase(int numOfArgs, char* airports[]);
char** printFlightsToAirport(char* airportName, DB* db, int* logSize, int mission);
char* printFlightsData(FlightData object, int mission);
void runRequestOnDB(char* parameters[], int numOfParameters, DB* db, int pipeToParent[2], int mission);
char* compareFlights(DB* db, int *a, int *d, int airport);
void findAirCrafts(char** aircraft, int nofAirCrafts, DB* db, char*** output, int* logSize, int* phySize);
void reRunScript(DB* db);

#endif
