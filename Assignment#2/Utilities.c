#include "Utilities.h"

//////////////////////////////Data Base Functions//////////////////////////////


//free Data base
void freeDataBase(DB* db)
{
    for (int i = 0; i < db->nofAirports; i++) {
        free(db->airPortsNames[i]);
    }
    free(db->airPortsNames);

    for (int i = 0; i < db->nofAirports; i++)
    {
            free(db->airPortsArr[i].arivals);
            free(db->airPortsArr[i].Departurs);
    }
}

//build data base
DB* getDataBase(int numOfArgs, char* airports[])
{
    DB* DataBase;
    DataBase->nofAirports = numOfArgs;
    DataBase->airPortsArr = (AirPorts*)malloc(sizeof(AirPorts) * numOfArgs);
    checkAllocation(DataBase->airPortsArr);
   

    //reorder airporst names:
    DataBase->airPortsNames = reorderStringArray(numOfArgs, airports);

    //get the data base for each airport
    for (int q = 0; q < numOfArgs; q++)
    {   
        FILE* arrivalsFile, * departuresFile;
        char firstRowA[MAX_SIZE];
        char firstRowD[MAX_SIZE];
        char arrivals[MAX_SIZE];
        char departures[MAX_SIZE];

        openFilesByAirportName(DataBase->airPortsNames[q], &departuresFile, &arrivalsFile);

        DataBase->airPortsArr[q].SizeArivals = howManyRowsInFile(arrivalsFile);
        DataBase->airPortsArr[q].SizeDeparturs = howManyRowsInFile(departuresFile);

        fgets(firstRowA, MAX_SIZE, arrivalsFile);
        fgets(firstRowD, MAX_SIZE, departuresFile);

        for (int i = 0; i < DataBase->airPortsArr[q].SizeArivals; i++)
        {
            fgets(arrivals, MAX_SIZE, arrivalsFile);
            DataBase->airPortsArr[q].arivals[i] = splitS(arrivals);
            DataBase->airPortsArr[q].arivals[i].arrivalOrDeparture = ARRIVAL;
        }

        for (int i = 0; i < (DataBase->airPortsArr[q].SizeDeparturs); i++)
        {
            fgets(departures, MAX_SIZE, departuresFile);
            DataBase->airPortsArr[q].Departurs[i] = splitS(departures);
            DataBase->airPortsArr[q].Departurs[i].arrivalOrDeparture = DEPARTURE;
        }

        fclose(arrivalsFile);
        fclose(departuresFile);
    }

    return DataBase;
}

void swap(char* arr[], int i, int j)
{
    char* temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

int partition(char* arr[], int low, int high)
{
    int pivot_length = strlen(arr[high]);
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (strlen(arr[j]) <= pivot_length) {
            i++;
            swap(arr, i, j);
        }
    }
    swap(arr, i + 1, high);
    return (i + 1);
}

void quickSort(char* arr[], int low, int high)
{
    if (low < high) {
        int pivot = partition(arr, low, high);
        quickSort(arr, low, pivot - 1);
        quickSort(arr, pivot + 1, high);
    }
}

char** reorderStringArray(int numOfArgs, char* airports[])
{
    // Create a new array to store the reordered strings
    char** reordered = (char**)malloc(numOfArgs * sizeof(char*));
    checkAllocation(reordered);
    for (int i = 0; i < numOfArgs; i++) {
        reordered[i] = (char*)malloc((strlen(airports[i]) + 1) * sizeof(char));
        checkAllocation(reordered[i]);
        strcpy(reordered[i], airports[i]);
    }

    quickSort(reordered, 0, numOfArgs - 1);

    return reordered;
}

//////////////////////////////General Functions//////////////////////////////

//this is a search function to find the needed airport
int quickSearch(char* arr[], int size, char* target) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (strcmp(arr[mid], target) == 0) {
            return mid;  // Found the target at index mid
        }

        if (strcmp(arr[mid], target) < 0) {
            left = mid + 1;  // Target is in the right half
        }
        else {
            right = mid - 1;  // Target is in the left half
        }
    }

    return -1;  // Target not found
}


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
void printFlightsToAirport(char* airportName, DB db) //Prints all flights details to airportName.
{
    int j = quickSearch(db.airPortsNames, db.nofAirports, airportName);
    if(j != -1)
    { 
        printf("-------------------------%s-------------------------\n", airportName);
        for (int i = 0; i < db.airPortsArr[j].SizeArivals; i++)
        {
                printFlightsData(db.airPortsArr[j].arivals[i]);
        }
    }
    else
    {
        printf("\n%s does not exist in data base", airportName);
    }
}

//this function prints flights data
void printFlightsData(FlightData object)
{
    printf("Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
           object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
}

void runQ1(char* parameters[], int numOfParameters, DB db)
{
    for (int i = 1; i < numOfParameters; i++)
    {
        printFlightsToAirport(parameters[i], db);
    }
}
//////////////////////////////Q2 Functions//////////////////////////////
//this function gets Airport name
//prints its schedule in order of time
void printAirportSchedule(char* airportName, DB db)
{
    int j = quickSearch(db.airPortsNames, db.nofAirports, airportName);
    int size = db.airPortsArr[j].SizeArivals + db.airPortsArr[j].SizeDeparturs;
    FlightData* data = (FlightData*)malloc(sizeof(FlightData) * (size));
    checkAllocation(data);

    for (int a = 0, d = 0; (a+d) < size;)
    {
        if (a == db.airPortsArr[j].SizeArivals)
        {
            printFullSchedule(db.airPortsArr[j].Departurs[d]);
            d++;
        }
        if (d == db.airPortsArr[j].SizeDeparturs)
        {
            printFullSchedule(db.airPortsArr[j].arivals[a]);
            a++;
        }
        if (strcmp(db.airPortsArr[j].arivals[a].lastSeen, db.airPortsArr[j].Departurs[d].firstSeen) < 0)
        {
            printFullSchedule(db.airPortsArr[j].Departurs[d]);
            d++;
        }
        else
        {
            printFullSchedule(db.airPortsArr[j].arivals[a]);
            a++;
        }
    }
}

//this function compares Flights
//int compareFlights(const void* a, const void* b)
//{
//    const FlightData* first = (const void*) a;
//    const FlightData* second = (const void*) b;
//    int output = 0;
//
//    if (first->arrivalOrDeparture == ARRIVAL && second->arrivalOrDeparture == ARRIVAL)
//    {
//        output = strcmp(first->lastSeen, second->lastSeen);
//    }
//
//    else if (first->arrivalOrDeparture == ARRIVAL && second->arrivalOrDeparture == DEPARTURE)
//    {
//        output = strcmp(first->lastSeen, second->firstSeen);
//    }
//
//    else if (first->arrivalOrDeparture == DEPARTURE && second->arrivalOrDeparture == ARRIVAL)
//    {
//        output = strcmp(first->firstSeen, second->lastSeen);
//    }
//
//    else
//    {
//        output = strcmp(first->firstSeen, second->firstSeen);
//    }
//
//    return output;
//}

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

void runQ2(char* parameters[], int numOfParameters, DB db)
{
        for (int i = 1; i < numOfParameters; i++)
    {
        printAirportSchedule(parameters[i], db);
    }
}
//////////////////////////////Q3 Functions//////////////////////////////
//this function gets and open file of specific airport, the aircraft searched for and their number
//returns all flights for said aircraft in given airport
void findAirCrafts(char** aircraft, int nofAirCrafts, DB db)
{
    for (int j = 0; j < db.nofAirports; j++)
    {
        for (int k = 0; k < db.airPortsArr[j].SizeArivals; k++)
        {
            for (int t = 0; t < nofAirCrafts; t++)
            {
                if (strcmp(db.airPortsArr[j].arivals[k].icao24, aircraft[t + 1]) == 0)
                {
                    printQ3(db.airPortsArr[j].arivals[k]);
                }
            }
        }
        for (int k = 0; k < db.airPortsArr[j].SizeDeparturs; k++)
        {
            for (int t = 0; t < nofAirCrafts; t++)
            {
                if (strcmp(db.airPortsArr[j].Departurs[k].icao24, aircraft[t + 1]) == 0)
                {
                    printQ3(db.airPortsArr[j].Departurs[k]);
                }
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

void runQ3(char* parameters[], int numOfParameters, DB db)
{
    findAirCrafts(parameters, numOfParameters, db);
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