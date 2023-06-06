#include "Utilities.h"

//////////////////////////////Data Base Functions//////////////////////////////

int checkRows(FILE* file)
{
    printf("In checkRows!\n");
    int counter = 0;
    char input;

    for (input = getc(file); input != EOF; input = getc(file))
    {
        if (input == '\n')
            counter++;
    }

    fseek(file, 0, SEEK_SET);

    printf("Exited checkRows with value %d\n", counter - 1);
    return (counter - 1);
}


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

    free(db);
}

//build data base
DB* getDataBase(int numOfArgs, char* airports[])
{
    int debug = 0;
    printf("Am in getDB\n");
    printf("Got arguments: %d\n", numOfArgs);
    for (int i = 0; i < numOfArgs; i++)
        printf("%s\n", airports[i]);
    printf("Breakpoint#%d\n", debug++);

    DB* DataBase = (DB*)malloc(sizeof(DB));
    checkAllocation(DataBase);
    DataBase->nofAirports = numOfArgs;
    DataBase->airPortsArr = (AirPorts*)malloc(sizeof(AirPorts) * numOfArgs);
    checkAllocation(DataBase->airPortsArr);
    printf("Breakpoint#%d\n", debug++);

    //reorder airporst names:
    DataBase->airPortsNames = reorderStringArray(numOfArgs, airports);
    printf("Breakpoint#%d\n", debug++);

    //get the data base for each airport
    for (int q = 0; q < numOfArgs; q++)
    {
        FILE* arrivalsFile, * departuresFile;
        char firstRowA[MAX_SIZE];
        char firstRowD[MAX_SIZE];
        char arrivals[MAX_SIZE];
        char departures[MAX_SIZE];

        openFilesByAirportName(DataBase->airPortsNames[q], &departuresFile, &arrivalsFile);
        printf("Breakpoint#%d\n", debug++); //We get here!

        //Option 1: call howManyRows - doesn't work
        //Option 2: call checkRows - doesn't work
        //Option 3: copy checkRows to here - STILL DOESN'T WORK!!

        DataBase->airPortsArr[q].SizeDeparturs = checkRows(departuresFile);
        DataBase->airPortsArr[q].SizeArivals = checkRows(arrivalsFile);
        printf("Breakpoint#%d\n", debug++);

        fgets(firstRowA, MAX_SIZE, arrivalsFile);
        fgets(firstRowD, MAX_SIZE, departuresFile);
        printf("Breakpoint#%d\n", debug++);

        DataBase->airPortsArr[q].arivals = (FlightData*)malloc(sizeof(FlightData) *
                (DataBase->airPortsArr[q].SizeArivals));
        DataBase->airPortsArr[q].Departurs = (FlightData*)malloc(sizeof(FlightData) *
                (DataBase->airPortsArr[q].SizeDeparturs));
        checkAllocation(DataBase->airPortsArr[q].arivals);
        checkAllocation(DataBase->airPortsArr[q].Departurs);

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
        printf("Breakpoint#%d\n", debug++);

        fclose(arrivalsFile);
        fclose(departuresFile);
    }

    printf("Exit getDB\n");
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
    printf("In reorder!\n");
    int debug = 0;
    // Create a new array to store the reordered strings
    char** reordered = (char**)malloc(numOfArgs * sizeof(char*));
    checkAllocation(reordered);
    printf("Breakpoint#%d\n", debug++);
    for (int i = 0; i < numOfArgs; i++) {
        reordered[i] = (char*)malloc((strlen(airports[i]) + 1) * sizeof(char));
        checkAllocation(reordered[i]);
        strcpy(reordered[i], airports[i]);
        printf("Breakpoint#%d\n", debug++);
    }

    quickSort(reordered, 0, numOfArgs - 1);
    printf("Exit reorder!\n");
    for (int i = 0; i < numOfArgs; i++)
        printf("%s\n", reordered[i]);

    return reordered;
}

//////////////////////////////General Functions//////////////////////////////
void ReadOrWriteToPipe(char** output, int O_size, int pipe[2], bool SIG)
{
    int currentStrSize = 0;
    if (SIG == 0)//read
    {
        for (int i = 0; i < O_size; i++)
        {
            read(pipe[0], &currentStrSize, sizeof(int));
            output[i] = (char*)malloc(sizeof(char) * (currentStrSize + 1));
            checkAllocation(output[i]);
            read(pipe[0], output[i], currentStrSize);
            output[i][currentStrSize] = '\0';
        }
    }
    else // write
    {
        for (int i = 0; i < O_size; i++)
        {
            currentStrSize = strlen(output[i]);
            write(pipe[1], &currentStrSize, sizeof(int));
            write(pipe[1], output[i], currentStrSize);
            output[i][currentStrSize] = '\0';
        }
    }

}
void child_process(int pipeToChild[2], int pipeToParent[2], int number, DB* dataBase)
{
    printf("Reached child process with choice %d\n", number);
    int arrSize = 0, currentStrSize = 0;
    char** output = 0;
    pid_t childId;

    if (number >= 1 && number <= 3)
    {
        read(pipeToChild[0], &arrSize, sizeof(int));
        printf("Got arrSize: %d\n", arrSize);
        output = (char**)malloc(sizeof(char*) * arrSize);
        checkAllocation(output);

        for (int i = 0; i < arrSize; i++)
        {
            read(pipeToChild[0], &currentStrSize, sizeof(int));
            printf("Got string size: %d\n", currentStrSize);
            output[i] = (char*)malloc(sizeof(char) * (currentStrSize + 1));
            checkAllocation(output[i]);
            read(pipeToChild[0], output[i], currentStrSize);
            output[i][currentStrSize] = '\0';
        }

        printf("Got output arr: %s\n", output[0]);
    }

    switch(number)
    {
        case 1:
            runQ1(output, arrSize, *dataBase, pipeToParent);
            break;
        case 2:
            runQ2(output, arrSize, *dataBase, pipeToParent);
            break;
        case 3:
            runQ3(output, arrSize, *dataBase, pipeToParent);
        case 4:

        case 5:

        case 6:

        case 7:

        default:
            break;

    }

    for (int i = 0; i < arrSize; i++)
    {
        free(output[i]);
    }

    if (arrSize > 0)
        free(output);
}

void graceful_exit_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Graceful exit signal received\n");
        // zip_db_files(); // Assuming this function takes care of the DB zipping
        exit(0);
    }
}

void sigint_handler(int signum)
{
    if (signum == SIGINT)
    {
        printf("CTRL+C signal received\n");
        kill(getpid(), SIGUSR1);
    }
}


void printMenu()
{
    printf("Menu:\n");
    printf("1. Receive airports ICOA code names and output all arrival flights and flight details.\n");
    printf("2. Receive airports name and output the full airport schedule (departures and arrivals) ordered by time.\n");
    printf("3. Receive list of aircraft names (icao24) and output all flights (departures and arrivals) that it has made.\n");
    printf("4. Update the existing airports in the DB with recent data (rerun BASH script).\n");
    printf("5. Zip the DB files.\n");
    printf("6. Get child process's ID.\n");
    printf("7. Graceful exit - child shall zip the DB files and terminate (think about collecting the exit status of the child process).\n");
    printf("Enter your choice (1-7): ");
}

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
    printf("In splitS!\n");
    FlightData FD;

    strcpy(FD.icao24, strtok(str, ","));
    strcpy(FD.firstSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(FD.departureAirPort, strtok(NULL, ","));
    strcpy(FD.lastSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(FD.arrivalAirPort, strtok(NULL, ","));
    strcpy(FD.flightNumber, strtok(NULL, ",\n"));

    printf("Exiting splitS!\n");
    return FD;
}

//this function checks how many flights are in a specific file
int howManyRowsInFile(FILE* file)
{
    printf("In how many rows!");
    int counter = 0;
    char input;

    for (input = getc(file); input != EOF; input = getc(file))
    {
        if (input == '\n')
            counter++;
    }

    fseek(file, 0, SEEK_SET);

    printf("Exited numOfRows with value %d", counter - 1);
    return counter - 1;
}

void checkAllocation(void* pointer)
{
    if (pointer == NULL)
        exit(-1);
}

void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile) //Opens an airport's database.
{
    printf("In openfiles!\n");
    int debug = 0;
    char dir1[MAX_SIZE] = "../flightsDB/";
    char dir2[MAX_SIZE];
    strcat(dir1, airportName);
    strcat(dir1, "/");
    strcat(dir1, airportName);
    strcpy(dir2, dir1);

    strcat(dir1, ".dpt");
    strcat(dir2, ".arv");

    printf("Breakpoint#%d\n", debug++);
    printf("%s\n", dir1);
    printf("%s\n", dir1);
    *departureFile = fopen(dir1, "r");
    *arrivalFile = fopen(dir2, "r");
    printf("Exit open file\n");
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
    printf("I'm in DirList\n");
    int debug = 0;
    char** output = NULL;
    int phySize = 1, logSize = 0;

    output = (char**)malloc(sizeof(char*) * phySize);
    checkAllocation(output);
    printf("Breakpoint#%d\n", debug++);

    DIR* directory = opendir("../flightsDB/");
    checkAllocation(directory);
    printf("Breakpoint#%d\n", debug++);

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        printf("Breakpoint#%d\n", debug++);
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
    printf("Breakpoint#%d\n", debug++);

    output = (char**)realloc(output, logSize * sizeof(char*));
    checkAllocation(output);
    *size = logSize;
    printf("Breakpoint#%d\n", debug++);

    closedir(directory);
    printf("I exit DirList\n");
    return output;
}

bool isValidChar(char input)
{
    return (input == ',' || input == '\n'|| (input >= 65 && input <= 122));
}

char** readInput(int* size)
{
    int arrLogSize = 0, arrPhySize = 0, currentStringLogSize = 0, currentStringPhySize = 0;
    char** output = NULL;

    printf("Please enter desired input seperated by a single comma (,).\n"
           "For example: LLBG,LLTK\n\n");

    char input = getchar();

    if (input == '\n') //case of previous buffer.
    {
        input = getchar();
    }

    while (input != '\n')
    {
        while (!isValidChar(input))
        {
            input = getchar();
        }

        if (input >= 97)
        {
            input -= 32;
        }

        else if (input == '\n')
        {
            continue;
        }

        if (arrLogSize == arrPhySize)
        {
            if (arrPhySize == 0)
            {
                arrPhySize += 1;
                output = (char**)malloc(sizeof(char*));
            }

            else
            {
                arrPhySize *= 2;
                output = (char**)realloc(output, arrPhySize * sizeof(char*));
            }

            checkAllocation(output);
        }

        if (currentStringLogSize == currentStringPhySize)
        {
            if (currentStringPhySize == 0)
            {
                currentStringPhySize += 4;
                output[arrLogSize] = (char*)malloc(sizeof(char) * currentStringPhySize);
                checkAllocation(output[arrLogSize]);
            }

            else if (currentStringPhySize > 0)
            {
                currentStringPhySize *= 2;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }
        }

        output[arrLogSize][currentStringLogSize] = input;
        currentStringLogSize++;
        input = getchar();

        if (input == ',' || input == '\n')
        {
            if (currentStringLogSize == currentStringPhySize)
            {
                currentStringPhySize += 1;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }

            else if (currentStringLogSize + 1 < currentStringPhySize)
            {
                currentStringPhySize = currentStringLogSize + 1;
                output[arrLogSize] = (char*)realloc(output[arrLogSize], currentStringPhySize * sizeof(char*));
                checkAllocation(output[arrLogSize]);
            }

            output[arrLogSize][currentStringLogSize] = '\0';
            arrLogSize++;
            currentStringLogSize = 0;
            currentStringPhySize = 0;

            if (input == ',')
            {
                input = getchar();
            }
            continue;
        }
    }

    if (arrPhySize > arrLogSize)
    {
        output = (char**)realloc(output, arrLogSize * sizeof(char*));
        checkAllocation(output);
    }

    *size = arrLogSize;
    return output;
}
//////////////////////////////Q1 Functions//////////////////////////////
char** printFlightsToAirport(char* airportName, DB db, int pipeToParent[2], int* logSize) //Prints all flights details to airportName.
{
    char** output = (char**)malloc(sizeof(char*) * MAX_SIZE);
    int phySize = MAX_SIZE;
    *logSize = 0;
    int currStringSize = 0;
    int j = quickSearch(db.airPortsNames, db.nofAirports, airportName);

    if (j != -1)
    {
        if ((*logSize) == phySize || db.airPortsArr[j].SizeArivals > phySize - (*logSize))
        {
            phySize *= 2;
            output = (char**)realloc(output, sizeof(char*) * phySize);
            checkAllocation(output);
        }

        currStringSize = snprintf(NULL, 0, "-------------------------%s-------------------------\n",
                                  airportName);
        output[*logSize] = (char*)malloc(currStringSize + 1);
        checkAllocation(output);
        snprintf(output[*logSize], currStringSize + 1, "-------------------------%s-------------------------\n",
                 airportName);
        (*logSize)++;
        for (int i = 0; i < db.airPortsArr[j].SizeArivals; i++)
        {
            output[*logSize] = printFlightsData(db.airPortsArr[j].arivals[i]);
            (*logSize)++;
        }
    }
    else
    {
        currStringSize = snprintf(NULL, 0, "\n%s does not exist in data base", airportName);
        output[*logSize] = (char*)malloc(currStringSize + 1);
        checkAllocation(output);
        snprintf(output[*logSize], currStringSize + 1, "\n%s does not exist in data base", airportName);
        (*logSize)++;
    }

    if (phySize > (*logSize))
    {
        output = (char**)realloc(output, sizeof(char*) * (*logSize));
        checkAllocation(output);
    }

    return output;
}

//this function prints flights data
char* printFlightsData(FlightData object)
{
    char* output = (char*)malloc(sizeof(char) * MAX_SIZE);
    checkAllocation(output);
    int logSize = 0;
    logSize = snprintf(NULL, 0, "Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
                       object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
    snprintf(output, logSize + 1, "Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
             object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);

    if (logSize < MAX_SIZE)
    {
        output = (char*)realloc(output, sizeof(char) * (logSize + 1));
        checkAllocation(output);
        output[logSize] = '\0';
    }

    return output;
}

void runQ1(char* parameters[], int numOfParameters, DB db, int pipeToParent[2])
{
    char** buffer = NULL, **output = NULL;
    int bufferLogSize = 0, bufferPhySize = MAX_SIZE;
    int outputLogSize = 0;
    buffer = (char**)malloc(sizeof(char*) * MAX_SIZE);
    checkAllocation(buffer);

    for (int i = 1; i < numOfParameters; i++)
    {
        output = printFlightsToAirport(parameters[i], db, pipeToParent, &outputLogSize);

        while (bufferPhySize - bufferLogSize < outputLogSize)
        {
            bufferPhySize = 2 * (outputLogSize - (bufferPhySize - bufferLogSize));
            buffer = (char**)realloc(buffer, sizeof(char*) * bufferPhySize);
            checkAllocation(buffer);
        }

        for (int i = 0; i < outputLogSize; i++)
        {
            output[bufferLogSize] = output[i];
            bufferLogSize++;
        }
    }

    if (bufferLogSize < bufferPhySize)
    {
        buffer = (char**)realloc(buffer, sizeof(char*) * bufferLogSize);
        checkAllocation(buffer);
    }

    write(pipeToParent[1], &bufferLogSize, sizeof(int));
    write(pipeToParent[1], &buffer, sizeof(char**) * bufferLogSize);
}
//////////////////////////////Q2 Functions//////////////////////////////
//this function gets Airport name
//prints its schedule in order of time
void printAirportSchedule(char* airportName, DB db, int pipeToParent[2])
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

void runQ2(char* parameters[], int numOfParameters, DB db, int pipeToParent[2])
{
    for (int i = 1; i < numOfParameters; i++)
    {
        printAirportSchedule(parameters[i], db, pipeToParent);
    }
}
//////////////////////////////Q3 Functions//////////////////////////////
//this function gets and open file of specific airport, the aircraft searched for and their number
//returns all flights for said aircraft in given airport
void findAirCrafts(char** aircraft, int nofAirCrafts, DB db, int pipeToParent[2])
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

void runQ3(char* parameters[], int numOfParameters, DB db, int pipeToParent[2])
{
    findAirCrafts(parameters, numOfParameters, db, pipeToParent);
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
