#ifndef UTILITIES_H
#define UTILITIES_H
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
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <zip.h>
#include <sys/stat.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/sem.h>

#define MAX_SIZE 300
#define FD_MAX 25
#define ARRIVAL 0
#define DEPARTURE 1
#define NAME_LEN 4
#define READ 0
#define WRITE 1
#define FINISH 1
#define EMPTY_INPUT -1
#define SCRIPT_FAILURE -2
#define FETCH_DATA 1
#define INCOMING_FLIGHTS 2
#define FULL_SCHEDULE 3
#define AIRCRAFT_SCHEDULE 4
#define ZIP_DB 5
#define SHUTDOWN 6

#define DB_SERVICE_SEMAPHORE "/dbservicesemaphore"
#define FLIGHTS_SERVICE_SEMAPHORE "/flightservicesemaphore"

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

//////////////////////////////DB Service Functions/////////////////////////////
void dbServiceProcess(int FIFO, int Opcode, DB** dataBase);
void ReadOrWriteToPipe(char** output, int O_size, int FIFO, bool SIG);
//////////////////////////////DB Functions/////////////////////////////////////
DB* getDataBase(int numOfArgs, char* airports[]);
char** reorderStringArray(int numOfArgs, char* airports[]);
char* unix_time_to_date(const char* timestamp_str);
void loadDatabase(int numOfArgs, char* airports[]);
char** printFlightsToAirport(char* airportName, DB* db, int* logSize, int mission);
char* printFlightsData(FlightData object, int mission);
void runRequestOnDB(char* parameters[], int numOfParameters, DB* db, int FIFO, int mission);
char* compareFlights(DB* db, int *a, int *d, int airport);
void findAirCrafts(char** aircraft, int nofAirCrafts, DB* db, char*** output, 
int* logSize, int* phySize);
DB* reRunScript(char** inputArr, int arrSize, DB* dataBase, int FIFO);
FlightData splitS(char* str);
void freeDataBase(DB* db);
//////////////////////////////File Functions///////////////////////////////////
int checkRows(FILE* file);
void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile);
//////////////////////////////Directory Functions//////////////////////////////
bool doesDBFolderExists();
char** createDirList(int* size);
void removeDirectory(const char* path);
//////////////////////////////Zip Functions////////////////////////////////////
bool doesZipExists();
void prepareToUnzip();
void prepareToZip();
void addFileToZip(struct zip* archive, const char* filePath, const char* entryName);
void addFolderToZip(struct zip* archive, const char* folderPath, const char* baseDir);
int zipFolder(const char* folderPath, const char* zipFilePath);
int unzipFolder(const char* zipFilePath, const char* destinationFolder);
//////////////////////////////General Functions//////////////////////////////
int compareStrings(const void* a, const void* b);
bool file_exists(const char* filename);
int quickSearch(char* arr[], int size, char* target);
void checkAllocation(void* ptr);
#endif