#include "Utilities.h"

//////////////////////////////Data Base Functions//////////////////////////////
//The equivalent of howManyRows.
int checkRows(FILE* file)
{
    int counter = 0;
    char input;

    for (input = (char)getc(file); input != EOF; input = (char)getc(file))
    {
        if (input == '\n')
            counter++;
    }

    fseek(file, 0, SEEK_SET);

    return (counter);
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
        free(db->airPortsArr[i].arrivals);
        free(db->airPortsArr[i].Departures);
    }

    free(db);
}

//build database
DB* getDataBase(int numOfArgs, char* airports[])
{
    DB* DataBase = (DB*)malloc(sizeof(DB));
    checkAllocation(DataBase);
    DataBase->nofAirports = numOfArgs;
    DataBase->airPortsArr = (AirPorts*)malloc(sizeof(AirPorts) * numOfArgs);
    checkAllocation(DataBase->airPortsArr);

    //reorder airports names
    DataBase->airPortsNames = reorderStringArray(numOfArgs, airports);

    //get the database for each airport
    for (int q = 0; q < numOfArgs; q++)
    {
        FILE* arrivalsFile, * departuresFile;
        char firstRowA[MAX_SIZE];
        char firstRowD[MAX_SIZE];
        char arrivals[MAX_SIZE];
        char departures[MAX_SIZE];

        openFilesByAirportName(DataBase->airPortsNames[q], &departuresFile, &arrivalsFile);

        DataBase->airPortsArr[q].SizeDepartures = checkRows(departuresFile);
        DataBase->airPortsArr[q].SizeArrivals = checkRows(arrivalsFile);

        fgets(firstRowA, MAX_SIZE, arrivalsFile);
        fgets(firstRowD, MAX_SIZE, departuresFile);

        DataBase->airPortsArr[q].arrivals = (FlightData*)malloc(sizeof(FlightData) *
                                                                (DataBase->airPortsArr[q].SizeArrivals));
        DataBase->airPortsArr[q].Departures = (FlightData*)malloc(sizeof(FlightData) *
                                                                  (DataBase->airPortsArr[q].SizeDepartures));
        checkAllocation(DataBase->airPortsArr[q].arrivals);
        checkAllocation(DataBase->airPortsArr[q].Departures);

        for (int i = 0; i < DataBase->airPortsArr[q].SizeArrivals; i++)
        {
            fgets(arrivals, MAX_SIZE, arrivalsFile);
            DataBase->airPortsArr[q].arrivals[i] = splitS(arrivals);
            DataBase->airPortsArr[q].arrivals[i].arrivalOrDeparture = ARRIVAL;
        }

        for (int i = 0; i < (DataBase->airPortsArr[q].SizeDepartures); i++)
        {
            fgets(departures, MAX_SIZE, departuresFile);
            DataBase->airPortsArr[q].Departures[i] = splitS(departures);
            DataBase->airPortsArr[q].Departures[i].arrivalOrDeparture = DEPARTURE;
        }

        fclose(arrivalsFile);
        fclose(departuresFile);
    }

    return DataBase;
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
    qsort(reordered, numOfArgs, sizeof(char*), compareStrings);

    return reordered;
}

//////////////////////////////General Functions//////////////////////////////
bool doesZipExists()
{
    bool output = false;
    DIR *dir;
    struct dirent *entry;

    dir = opendir("..");

    while (dir != NULL && (entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, "flightsDB.zip") == 0) {
            output = true;
            break;
        }
    }

    closedir(dir);
    return output;
}

// Comparison function for qsort
int compareStrings(const void* a, const void* b)
{
    const char** str1 = (const char**)a;
    const char** str2 = (const char**)b;
    return strcmp(*str1, *str2);
}

void ReadOrWriteToPipe(char** output, int O_size, int pipe[2], bool SIG)
{
    int currentStrSize = 0;

    if (SIG == READ)//read
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
            currentStrSize = (int)strlen(output[i]);
            write(pipe[1], &currentStrSize, sizeof(int));
            write(pipe[1], output[i], currentStrSize);
            output[i][currentStrSize] = '\0';
        }
    }
}

void child_process(int pipeToChild[2], int pipeToParent[2], int number, DB* dataBase)
{
    int arrSize = 0, exitCode;
    char** output = NULL;
    pid_t childId;

    if (number >= 1 && number <= 3 || number == GENERATE_DB)
    {
        read(pipeToChild[0], &arrSize, sizeof(int));
        output = (char**)malloc(sizeof(char*) * arrSize);
        checkAllocation(output);
        ReadOrWriteToPipe(output, arrSize, pipeToChild, READ);
        runRequestOnDB(output, arrSize, dataBase, pipeToParent, number);
    }

    switch(number)
    {
        case 4:
            reRunScript(dataBase);
            break;
        case 5:
            //zip DB
            break;
        case 6:
            childId = getpid();
            write(pipeToParent[1], &childId, sizeof(childId));
            break;
        case 7:
            //zip DB.
            exitCode = EXIT_SUCCESS;
            write(pipeToParent[1], &exitCode, sizeof(exitCode));
            freeDataBase(dataBase);
            exit(EXIT_SUCCESS);
        case GENERATE_DB:
            loadDatabase(arrSize, output);
            break;
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
        // zip_db_files();
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
    printf("7. Graceful exit - child shall zip the DB files and terminate.\n");
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
    FlightData FD;

    strcpy(FD.icao24, strtok(str, ","));
    strcpy(FD.firstSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(FD.departureAirPort, strtok(NULL, ","));
    strcpy(FD.lastSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(FD.arrivalAirPort, strtok(NULL, ","));
    strcpy(FD.flightNumber, strtok(NULL, ",\n\r"));

    return FD;
}

void checkAllocation(void* pointer)
{
    if (pointer == NULL)
        exit(-1);
}

void openFilesByAirportName(char* airportName, FILE** departureFile, FILE** arrivalFile) //Opens an airport's database.
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

void loadDatabase(int numOfArgs, char* airports[]) //Loads database according to arguments.
{
    char command[MAX_SIZE] = "bash ../flightScanner.sh";
    for (int i = 0; i < numOfArgs; i++)
    {
        strcat(command," ");
        strcat(command,airports[i]);
    }

    system(command);
}

//this function gets and open file of specific airport, the aircraft searched for and their number
//returns all flights for said aircraft in given airport
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
    if (strftime(buffer, 80, "%Y-%m-%d %H:%S:%M\0", timeinfo_target) == 0) {
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

    DIR* directory = opendir("../flightsDB/");
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

bool isValidChar(char input)
{
    return (input == ',' || input == '\n'|| (input >= 65 && input <= 122) || (input >= 48 && input <= 57));
}

char** readInput(int* size, int choice)
{
    int arrLogSize = 0, arrPhySize = 0, currentStringLogSize = 0, currentStringPhySize = 0;
    char** output = NULL;

    printf("Please enter desired input seperated by a single comma (,).\n"
           "For example: LLBG,LLTK\n\n");

    char input = (char)getchar();

    if (input == '\n') //case of previous buffer.
    {
        input = (char)getchar();
    }

    while (input != '\n')
    {
        while (!isValidChar(input))
        {
            input = (char)getchar();
        }

        if (input >= 97 && choice != 3)
        {
            input -= 32;
        }

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
                input = (char)getchar();
            }
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
        input = (char)getchar();

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
                input = (char)getchar();
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
char** printFlightsToAirport(char* airportName, DB* db, int* logSize ,int mission) //Prints all flights details to airportName.
{
    char** output = (char**)malloc(sizeof(char*) * MAX_SIZE);
    int phySize = MAX_SIZE;
    *logSize = 0;
    int currStringSize = 0;
    int j = quickSearch(db->airPortsNames, db->nofAirports, airportName);

    if (j != -1)
    {
        currStringSize = snprintf(NULL, 0, "\n-------------------------%s-------------------------\n", airportName);
        output[*logSize] = (char*)malloc(currStringSize + 1);
        checkAllocation(output);
        snprintf(output[*logSize], currStringSize + 1, "\n-------------------------%s-------------------------\n", airportName);
        (*logSize)++;
        if (mission == MISSION1)
        {
            for (int i = 0; i < db->airPortsArr[j].SizeArrivals; i++)//for each arrival
            {
                if(*logSize == phySize)
                {
                    phySize *= 2;
                    output = (char**)realloc(output, sizeof(char*) * phySize);
                    checkAllocation(output);
                }
                output[*logSize] = printFlightsData(db->airPortsArr[j].arrivals[i], MISSION1);
                (*logSize)++;
            }
        }

        else if (mission == MISSION2)
        {
            int size = db->airPortsArr[j].SizeArrivals + db->airPortsArr[j].SizeDepartures;
            int a = 0, d = 0;
            for (;(a+d) < size;) //for each arrival and departure
            {
                if(*logSize == phySize)
                {
                    phySize *= 2;
                    output = (char**)realloc(output, sizeof(char*) * phySize);
                    checkAllocation(output);
                }
                output[*logSize] = compareFlights(db, &a, &d, j);
                (*logSize)++;
            }
        }

    }
    else if (j == -1)
    {
        currStringSize = snprintf(NULL, 0, "\n%s does not exist in data base\n", airportName);
        output[*logSize] = (char*)malloc(currStringSize + 1);
        checkAllocation(output);
        snprintf(output[*logSize], currStringSize + 1, "\n%s does not exist in data base\n", airportName);
        (*logSize)++;
    }

    if (phySize > (*logSize))
    {
        output = (char**)realloc(output, sizeof(char*) * (*logSize));
        checkAllocation(output);
    }

    return output;
}

//this function prints flights data to a string and returns it
char* printFlightsData(FlightData object, int mission)
{
    char* output = (char*)malloc(sizeof(char) * MAX_SIZE);
    checkAllocation(output);
    int logSize = 0;

    switch (mission)
    {
        case MISSION1:
        {
            logSize = snprintf(NULL, 0, "Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
                               object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
            snprintf(output, logSize + 1, "Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
                     object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
        }
        case MISSION2:
        {
            if (object.arrivalOrDeparture == ARRIVAL)
            {
                logSize = snprintf(NULL, 0,"Flight #%-7s arriving from %s at %s\n", object.flightNumber, object.departureAirPort, object.lastSeen);
                snprintf(output, logSize + 1,"Flight #%-7s arriving from %s at %s\n", object.flightNumber, object.departureAirPort, object.lastSeen);
            }
            else if (object.arrivalOrDeparture == DEPARTURE)
            {
                logSize = snprintf(NULL, 0,"Flight #%-7s departing to %s at %s\n", object.flightNumber, object.arrivalAirPort, object.firstSeen);
                snprintf(output, logSize + 1,"Flight #%-7s departing to %s at %s\n", object.flightNumber, object.arrivalAirPort, object.firstSeen);
            }
            break;
        }
        case MISSION3:
        {
            logSize = snprintf(NULL, 0, "%s departed from %s at %s arrived in %s at %s\n", object.icao24, object.departureAirPort, object.firstSeen, object.arrivalAirPort, object.lastSeen);
            snprintf(output, logSize + 1, "%s departed from %s at %s arrived in %s at %s\n",
                     object.icao24, object.departureAirPort, object.firstSeen, object.arrivalAirPort, object.lastSeen);
            break;
        }

        default:
            break;
    }

    if (logSize < MAX_SIZE)
    {
        output = (char*)realloc(output, sizeof(char) * (logSize + 1));
        checkAllocation(output);
        output[logSize] = '\0';
    }

    return output;
}

void runRequestOnDB(char* parameters[], int numOfParameters, DB* db, int pipeToParent[2], int mission)
{
    char** buffer = NULL, **output = NULL;
    int bufferLogSize = 0, bufferPhySize = MAX_SIZE;
    int outputLogSize = 0;
    buffer = (char**)malloc(sizeof(char*) * MAX_SIZE);
    checkAllocation(buffer);
    if (mission == MISSION3)
    {
        findAirCrafts(parameters, numOfParameters, db, &buffer, &bufferLogSize, &bufferPhySize);
    }
    else
    {
        for (int i = 0; i < numOfParameters; i++)
        {//for each airport

            output = printFlightsToAirport(parameters[i], db, &outputLogSize, mission);

            while (bufferPhySize - bufferLogSize < outputLogSize)
            {
                bufferPhySize = 2 * (outputLogSize - (bufferPhySize - bufferLogSize));
                buffer = (char **) realloc(buffer, sizeof(char *) * bufferPhySize);
                checkAllocation(buffer);
            }
            for (int j = 0; j < outputLogSize; j++)
            {
                buffer[bufferLogSize] = output[j];
                bufferLogSize++;
            }
        }
    }

    if(bufferLogSize < bufferPhySize)
    {
        buffer = (char**)realloc(buffer, sizeof(char*) * bufferLogSize);
        checkAllocation(buffer);
    }

    write(pipeToParent[1], &bufferLogSize, sizeof(int));
    ReadOrWriteToPipe(buffer, bufferLogSize, pipeToParent, WRITE);

    for (int i = 0; i < bufferLogSize; i++)
    {
        free(buffer[i]);
    }

    free(buffer);
}
//////////////////////////////Q2 Functions//////////////////////////////
//this function gets Airport name
//prints its schedule in order of time
char* compareFlights(DB* db, int *a, int *d, int airport)
{
    if (*a == db->airPortsArr[airport].SizeArrivals)
    {
        (*d) +=1;
        return printFlightsData(db->airPortsArr[airport].Departures[(*d)-1], MISSION2);
    }
    if (*d == db->airPortsArr[airport].SizeArrivals)
    {
        (*a) +=1;
        return printFlightsData(db->airPortsArr[airport].arrivals[(*a)-1], MISSION2);
    }
    if (strcmp(db->airPortsArr[airport].arrivals[(*a)].lastSeen, db->airPortsArr[airport].Departures[(*d)].firstSeen) < 0)
    {
        (*d) +=1;
        return printFlightsData(db->airPortsArr[airport].Departures[(*d)-1], MISSION2);
    }
    else
    {
        (*a) +=1;
        return printFlightsData(db->airPortsArr[airport].arrivals[(*a) -1], MISSION2);
    }
}
//////////////////////////////Q3 Functions//////////////////////////////
//this function gets and open file of specific airport, the aircraft searched for and their number
//returns all flights for said aircraft in given airport
void findAirCrafts(char** aircraft, int nofAirCrafts, DB* db, char*** output, int* logSize, int* phySize)
{
    for (int j = 0; j < db->nofAirports; j++)
    {
        for (int k = 0; k < db->airPortsArr[j].SizeArrivals; k++)
        {
            for (int t = 0; t < nofAirCrafts; t++)
            {
                if (strcmp(db->airPortsArr[j].arrivals[k].icao24, aircraft[t]) == 0)
                {
                    if((*logSize) == *phySize)
                    {
                        (*phySize) *= 2;
                        (*output) = (char**)realloc((**output), sizeof(char*) * (*phySize));
                        checkAllocation(output);
                    }
                    (*output)[*logSize] = printFlightsData(db->airPortsArr[j].arrivals[k], MISSION3);
                    (*logSize)++;
                }
            }
        }
        for (int k = 0; k < db->airPortsArr[j].SizeDepartures; k++)
        {
            for (int t = 0; t < nofAirCrafts; t++)
            {
                if (strcmp(db->airPortsArr[j].Departures[k].icao24, aircraft[t]) == 0)
                {
                    if ((*logSize) == *phySize)
                    {
                        (*phySize) *= 2;
                        *output = (char**)realloc(*output, sizeof(char*) * (*phySize));
                        checkAllocation(output);
                    }
                    (*output)[*logSize] = printFlightsData(db->airPortsArr[j].Departures[k], MISSION3);
                    (*logSize)++;
                }
            }
        }
    }
}
//////////////////////////////Q4 Functions//////////////////////////////
void reRunScript(DB* db)
{
    char** APnames = db->airPortsNames;
    int size = db->nofAirports;
    freeDataBase(db);
    int sizeOfDirList = 0;
    char** dirList = createDirList(&sizeOfDirList);
    loadDatabase(sizeOfDirList,dirList);
    for (int i = 0; i < sizeOfDirList; i++)
    {
        free(dirList[i]);
    }
    free(dirList);
    db = getDataBase(size, APnames);
}
