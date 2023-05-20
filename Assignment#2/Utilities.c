#include "Utilities.h"

//////////////////////////////General Functions//////////////////////////////
//this function gets a string
//splits the string into type FlightData and returns it
FlightData splitS(char* str)
{
    FlightData FD;

    strcpy(FD.icao24, strtok(str, ","));
    strcpy(FD.firstSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(FD.departureAirPort, strtok(NULL, ","));
    strcpy(FD.lastSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(FD.arrivalAirPort, strtok(NULL, ","));
    strcpy(FD.flightNumber, strtok(NULL, ",\n"));

    return FD;
}

//this function checks how many flights are in a specific file
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
    char command[MAX_SIZE] = "bash flightScanner.sh";
    for (int i = 0; i < numOfArgs; i++)
    {
       strcat(command," ");
       strcat(command,airports[i]);
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
    checkAllocation(output);

    DIR* directory = opendir("flightsDB/");
    checkAllocation(directory);

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        // Exclude hidden files/directories that start with a dot
        if (entry->d_name[0] != '.' && strlen(entry->d_name) == NAME_LEN)
        {
            if (logSize == phySize)
            {
                phySize *= 2;
                output = (char**)realloc(output, phySize * sizeof(char*));
                checkAllocation(output);
            }

            output[logSize] = (char*)malloc(sizeof(char) * (NAME_LEN + 1));
            checkAllocation(output[logSize]);
            strcpy(output[logSize], entry->d_name);
            output[logSize][NAME_LEN] = '\0';

            logSize++;
        }
    }

    output = (char**)realloc(output, logSize * sizeof(char*));
    checkAllocation(output);
    *size = logSize;

    closedir(directory);
    return output;
}
//////////////////////////////Q1 Functions//////////////////////////////
void printFlightsToAirport(char* airportName) //Prints all flights details to airportName.
{
    FILE* arrivalsFile, *departuresFile;
    openFilesByAirportName(airportName, &departuresFile, &arrivalsFile);

    if (arrivalsFile)
    {
        int numOfFlights = howManyRowsInFile(arrivalsFile);
        char firstRow[MAX_SIZE];
        char otherRows[MAX_SIZE];
        fgets(firstRow, MAX_SIZE, arrivalsFile); //'flush' first row.

        FlightData* flightsData = (FlightData*)malloc(sizeof(FlightData) * numOfFlights);
        checkAllocation(flightsData);
        
        printf("-------------------------%s-------------------------\n",airportName);
        for (int i = 0; i < numOfFlights; i++)
        {
            fgets(otherRows, MAX_SIZE, arrivalsFile);
            flightsData[i] = splitS(otherRows);
            printFlightsData(flightsData[i]);
        }

        free(flightsData);
        fclose(arrivalsFile);
        fclose(departuresFile);
    }
}

//this function prints flights data
void printFlightsData(FlightData object)
{
    printf("Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
           object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
}

void runQ1(char* parameters[], int numOfParameters)
{
    for (int i = 1; i < numOfParameters; i++)
    {
        printFlightsToAirport(parameters[i]);
    }
}
//////////////////////////////Q2 Functions//////////////////////////////
//this function gets Airport name
//prints its schedule in order of time
void printAirportSchedule(char* airportName)
{
    FILE* arrivalsFile, *departuresFile;
    openFilesByAirportName(airportName, &departuresFile, &arrivalsFile);
    checkAllocation(arrivalsFile);
    checkAllocation(departuresFile);

    if (arrivalsFile && departuresFile)
    {
    int numOfArrivals = howManyRowsInFile(arrivalsFile);
    int numOfDepartures = howManyRowsInFile(departuresFile);

    char firstRowA[MAX_SIZE], firstRowD[MAX_SIZE];
    char arrivals[MAX_SIZE];
    char departures[MAX_SIZE];

    fgets(firstRowA, MAX_SIZE, arrivalsFile);
    fgets(firstRowD, MAX_SIZE, departuresFile);

    FlightData* data = (FlightData*)malloc(sizeof(FlightData) * (numOfArrivals + numOfDepartures));
    checkAllocation(data);

    for (int i = 0; i < numOfArrivals; i++)
    {
        fgets(arrivals, MAX_SIZE, arrivalsFile);
        data[i] = splitS(arrivals);
        data[i].arrivalOrDeparture = ARRIVAL;
    }

    for (int i = numOfArrivals; i < (numOfDepartures + numOfArrivals); i++)
    {
        fgets(departures, MAX_SIZE, departuresFile);
        data[i] = splitS(departures);
        data[i].arrivalOrDeparture = DEPARTURE;
    }

    qsort(data, numOfDepartures + numOfArrivals - 1, sizeof(FlightData), compareFlights);

    printf("-------------------------%s-------------------------\n",airportName);
    for (int i = 0; i < numOfDepartures + numOfArrivals; i++)
    {
        printFullSchedule(data[i]);
    }

    free(data);
    fclose(arrivalsFile);
    fclose(departuresFile);
    }
}

//this function compares Flights
int compareFlights(const void* a, const void* b)
{
    const FlightData* first = (const void*) a;
    const FlightData* second = (const void*) b;
    int output = 0;

    if (first->arrivalOrDeparture == ARRIVAL && second->arrivalOrDeparture == ARRIVAL)
    {
        output = strcmp(first->lastSeen, second->lastSeen);
    }

    else if (first->arrivalOrDeparture == ARRIVAL && second->arrivalOrDeparture == DEPARTURE)
    {
        output = strcmp(first->lastSeen, second->firstSeen);
    }

    else if (first->arrivalOrDeparture == DEPARTURE && second->arrivalOrDeparture == ARRIVAL)
    {
        output = strcmp(first->firstSeen, second->lastSeen);
    }

    else
    {
        output = strcmp(first->firstSeen, second->firstSeen);
    }

    return output;
}

//this function prints flight data 
void printFullSchedule(FlightData object)
{
    if (object.arrivalOrDeparture == ARRIVAL)
    {
        printf("Flight #%-7s arriving from %s at %s\n", object.flightNumber, object.departureAirPort, object.lastSeen);
    }

    else if (object.arrivalOrDeparture == DEPARTURE)
    {
        printf("Flight #%-7s departing to  %s at %s\n", object.flightNumber, object.arrivalAirPort, object.firstSeen);
    }
}

void runQ2(char* parameters[], int numOfParameters)
{
        for (int i = 1; i < numOfParameters; i++)
    {
        printAirportSchedule(parameters[i]);
    }
}
//////////////////////////////Q3 Functions//////////////////////////////
//this function gets and open file of specific airport, the aircraft searched for and their number
//returns all flights for said aircraft in given airport
void findAirCrafts(FILE* f, char** aircraft, int nofAirCrafts)
{
    int numOfFlights = howManyRowsInFile(f);
    char firstRow[MAX_SIZE], otherRows[MAX_SIZE];
    fgets(firstRow, MAX_SIZE, f); // get rid of info line

    for (int i = 0; i < numOfFlights; i++)
    {
        fgets(otherRows, MAX_SIZE, f);
        FlightData FD = splitS(otherRows);
        for (int j = 0; j < nofAirCrafts; j++)
        {
            if (strcmp(FD.icao24, aircraft[j + 1]) == 0)
            {
                printQ3(FD);
            }
        }
    }
}

//this function prints flight data
void printQ3(FlightData FD)
{
    printf("%s departed from %s at %s arrived in %s at %s\n",
           FD.icao24, FD.departureAirPort, FD.firstSeen, FD.arrivalAirPort, FD.lastSeen);
}

void runQ3(char* parameters[], int numOfParameters)
{
    FILE* arrivalFile, *departureFile;
    int sizeOfDirList = 0;
    char** dirList = createDirList(&sizeOfDirList);

    for (int i = 0; i < sizeOfDirList; i++)
    {
        openFilesByAirportName(dirList[i], &departureFile, &arrivalFile);
        findAirCrafts(departureFile, parameters, numOfParameters - 1);
        findAirCrafts(arrivalFile, parameters, numOfParameters - 1);

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
//////////////////////////////Q4 Functions//////////////////////////////
void runQ4()
{
    int sizeOfDirList = 0;
    char** dirList = createDirList(&sizeOfDirList);
    loadDatabase(sizeOfDirList,dirList);
    for (int i = 0; i < sizeOfDirList; i++)
    {
        free(dirList[i]);
    }
    free(dirList);
}
//////////////////////////////Q5 Functions//////////////////////////////
//////////////////////////////Q6 Functions//////////////////////////////
//////////////////////////////Q7 Functions//////////////////////////////