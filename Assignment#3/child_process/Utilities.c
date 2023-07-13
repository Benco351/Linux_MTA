#include "Utilities.h"

//////////////////////////////Data Base Functions//////////////////////////////
//Counts file's num of rows.
int checkRows(FILE* file)
{
    int counter = 0, debug = 0;
    char input, line[MAX_SIZE];

    while (fgets(line, sizeof(line), file) != NULL) 
    {
        counter++;
    }

    fseek(file, 0, SEEK_SET);
    return (counter - 1);
}

//free Data base
void freeDataBase(DB* DataBase)
{
    if (DataBase == NULL)
    {
        return; // No object to free
    }

    // Free the airports names array
    if (DataBase->airPortsNames != NULL)
    {
        for (int i = 0; i < DataBase->nofAirports; i++)
        {
            free(DataBase->airPortsNames[i]);
        }

        free(DataBase->airPortsNames);
    }

    // Free the flights data for each airport
    if (DataBase->airPortsArr != NULL)
    {
        for (int q = 0; q < DataBase->nofAirports; q++)
        {
            // Free arrivals
            if (DataBase->airPortsArr[q].arrivals != NULL)
            {
                free(DataBase->airPortsArr[q].arrivals);
            }
            
            // Free departures
            if (DataBase->airPortsArr[q].Departures != NULL)
            {
                free(DataBase->airPortsArr[q].Departures);
            }
        }
        free(DataBase->airPortsArr);
    }

    // Free the DB object itself
    free(DataBase);
}

//build database
DB* getDataBase(int numOfArgs, char* airports[])
{
    DB* DataBase = (DB*)malloc(sizeof(DB));
    checkAllocation(DataBase);
    DataBase->nofAirports = numOfArgs;
    DataBase->airPortsArr = (AirPorts*)malloc(sizeof(AirPorts) * numOfArgs);
    checkAllocation(DataBase->airPortsArr);

    DataBase->airPortsNames = reorderStringArray(numOfArgs, airports);
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

        if (DataBase->airPortsArr[q].SizeDepartures >  0)
        {
            DataBase->airPortsArr[q].Departures = (FlightData*)malloc(sizeof(FlightData) *
            (DataBase->airPortsArr[q].SizeDepartures));
            checkAllocation(DataBase->airPortsArr[q].Departures);
        }

        if (DataBase->airPortsArr[q].SizeArrivals > 0)
        {
            DataBase->airPortsArr[q].arrivals = (FlightData*)malloc(sizeof(FlightData) *
            (DataBase->airPortsArr[q].SizeArrivals));
            checkAllocation(DataBase->airPortsArr[q].arrivals);
        }

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
    for (int i = 0; i < numOfArgs; i++)
    {
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
bool doesDBFolderExists()
{
    bool output = false;
    DIR *dir;
    struct dirent *entry;

    dir = opendir("..");

    while (dir != NULL && (entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, "flightsDB") == 0) {
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

void ReadOrWriteToPipe(char** output, int O_size, int fd, bool SIG)
{
    int currentStrSize = 0, bytesCounter = 0;

    if (SIG == READ)
    {
        for (int i = 0; i < O_size; i++)
        {
            bytesCounter = read(fd, &currentStrSize, sizeof(int));
            while (bytesCounter != sizeof(int))
            {
                bytesCounter += read(fd, &currentStrSize, sizeof(int));
            }

            output[i] = (char*)malloc(sizeof(char) * (currentStrSize + 1));
            checkAllocation(output[i]);

            bytesCounter = read(fd, output[i], currentStrSize);
            while (bytesCounter != currentStrSize)
            {
                bytesCounter += read(fd, output[i], currentStrSize);
            }
            output[i][currentStrSize] = '\0';
        }
    }

    else
    {
        for (int i = 0; i < O_size; i++)
        {
            currentStrSize = (int)strlen(output[i]);
            bytesCounter = write(fd, &currentStrSize, sizeof(currentStrSize));
            while (bytesCounter != sizeof(currentStrSize))
            {
                bytesCounter += write(fd, &currentStrSize, sizeof(currentStrSize));
            }

            bytesCounter = write(fd, output[i], currentStrSize);
            while (bytesCounter != currentStrSize)
            {
                bytesCounter += write(fd, output[i], currentStrSize);
            }
            output[i][currentStrSize] = '\0';
        }
    }
}

void child_process(int fd, int Opcode, DB** dataBase)
{
    int arrSize = 0, exitCode, result = 0, bytesCounter = 0;
    char** output = NULL;
    pid_t childId;
    DB* newDB = NULL;

    if (Opcode >= 2 && Opcode <= 4)
    {
        bytesCounter = read(fd, &arrSize, sizeof(int));
        while (bytesCounter != sizeof(int))
        {
            bytesCounter += read(fd, &arrSize, sizeof(int));
        }

        output = (char**)malloc(sizeof(char*) * arrSize);
        checkAllocation(output);
        
        ReadOrWriteToPipe(output, arrSize, fd, READ);
        runRequestOnDB(output, arrSize, *dataBase, fd, Opcode);

        for (int i = 0; i < arrSize; i++)
        {
            free(output[i]);
        }

        if (arrSize > 0)
            free(output);
    }

    switch (Opcode)
    {
        case 1:
            newDB = reRunScript(*dataBase, fd);
            freeDataBase(*dataBase);
            *dataBase = newDB;
            result = FINISH;
            bytesCounter = write(fd, &result, sizeof(result));
            while (bytesCounter != sizeof(result))
            {
                bytesCounter += write(fd, &result, sizeof(result));
            }
            break;
        case 5:
            prepareToZip();
            result = FINISH;
            bytesCounter = write(fd, &result, sizeof(result));
            while (bytesCounter != sizeof(result))
            {
                bytesCounter += write(fd, &result, sizeof(result));
            }
            break;
        case 6:
            exitCode = EXIT_SUCCESS;
            bytesCounter = write(fd, &exitCode, sizeof(exitCode));
            while (bytesCounter != sizeof(exitCode))
            {
                bytesCounter += write(fd, &exitCode, sizeof(exitCode));
            }

            freeDataBase(*dataBase);
            close(fd);
            break;
        default:
            break;
    }
}

void prepareToZip()
{
    char tmp[MAX_SIZE], cwd[MAX_SIZE];
    int size = 0;
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        int size = strlen(cwd);
        cwd[size - 6] = '\0';
        strcpy(tmp,cwd);
        strcat(tmp,"/flightsDB.zip");
        strcat(cwd,"/flightsDB");
        bool DBFolderExists = doesDBFolderExists();

        if (DBFolderExists)
        {
            (void)zipFolder(cwd, tmp);
        }  
    }
}

void prepareToUnzip()
{
    char cwd[MAX_SIZE], tmp[MAX_SIZE];
    getcwd(cwd, sizeof(cwd));
    int size = strlen(cwd);
    cwd[size - 6] = '\0';
    strcpy(tmp, cwd);
    strcat(tmp, "/flightsDB.zip");
    strcat(cwd, "/flightsDB");
    bool DBFolderExists = doesDBFolderExists();

    if (!DBFolderExists)
    {
        mkdir(cwd,0777);
    }

    bool zipExists = doesZipExists();
    if (zipExists)
    {  
        if (cwd != NULL)
        {
            (void)unzipFolder(tmp, cwd);
        }
    }
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
    {
        printf("Memory allocation failed. Exiting program...\n");
        exit(-1);
    }
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
    char workDir[MAX_SIZE], commandLine[MAX_SIZE], tempStr[MAX_SIZE];
    strcpy(commandLine, "bash ");
    getcwd(workDir, sizeof(workDir));
    int size = strlen(workDir);
    workDir[size - 6] = '\0';
    strcpy(tempStr, workDir);
    strcat(tempStr, "/flightScanner.sh");
    strcat(commandLine, tempStr);

    for (int i = 0; i < numOfArgs; i++)
    {
        strcat(commandLine, " ");
        strcat(commandLine, airports[i]);
    }

    system(commandLine);
    sleep(5);
    system("rm -rf ../flightsDB/");
    system("mv flightsDB ..");
}

//this function gets and open file of specific airport, the aircraft searched for and their number
//returns all flights for said aircraft in given airport
char* unix_time_to_date(const char* timestamp_str)
{
    time_t timestamp = atoi(timestamp_str);
    struct tm* timeinfo_local;
    struct tm* timeinfo_target;
    char* buffer = (char*)malloc(sizeof(char)*80);

    // Convert Unix timestamp to struct tm in local timezone
    timeinfo_local = localtime(&timestamp);
    if (timeinfo_local == NULL)
    {
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
    if (strftime(buffer, 80, "%Y-%m-%d %H:%S:%M", timeinfo_target) == 0)
    {
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

    if (logSize > 0)
    {
        output = (char**)realloc(output, logSize * sizeof(char*));
        checkAllocation(output);
    }

    *size = logSize;
    closedir(directory);
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
        currStringSize = snprintf(NULL, 0, "%s does not exist in data base\n", airportName);
        output[*logSize] = (char*)malloc(currStringSize + 1);
        checkAllocation(output);
        snprintf(output[*logSize], currStringSize + 1, "%s does not exist in data base\n", airportName);
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

void runRequestOnDB(char* parameters[], int numOfParameters, DB* db, int fd, int mission)
{
    char** buffer = NULL, **output = NULL;
    int bufferLogSize = 0, bufferPhySize = MAX_SIZE, outputLogSize = 0, bytesCounter = 0;
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

    if (bufferLogSize > 0 && bufferLogSize < bufferPhySize)
    {
        buffer = (char**)realloc(buffer, sizeof(char*) * bufferLogSize);
        checkAllocation(buffer);
    }

    bytesCounter = write(fd, &bufferLogSize, sizeof(int));
    while (bytesCounter != sizeof(int))
    {
        bytesCounter += write(fd, &bufferLogSize, sizeof(int));
    }
    ReadOrWriteToPipe(buffer, bufferLogSize, fd, WRITE);

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
DB* reRunScript(DB* dataBase, int fd) //Re-runs the script and creates a new DB.
{
    DB* output = NULL;
    int BytesCounter = 0, nofAirports = 0, stringSize = 0;
    char** airports = NULL;

    BytesCounter = read(fd, &nofAirports, sizeof(int));
    while (BytesCounter != sizeof(nofAirports))
    {
        BytesCounter += read(fd, &nofAirports, sizeof(int));
    }

    airports = (char**)malloc(nofAirports * sizeof(char*));
    checkAllocation(airports);

    ReadOrWriteToPipe(airports, nofAirports, fd, READ);
    loadDatabase(nofAirports, airports);
    output = getDataBase(nofAirports, airports);

    return output;
}
/////////////////////////////////////ZIP Functions//////////////////////////////////

void addFileToZip(struct zip* archive, const char* filePath, const char* entryName)
{
    struct zip_source* source = zip_source_file_create(filePath, 0, -1, 0);
    if (!source) 
    {
        return;
    } 

    int result = zip_add(archive, entryName, source);
    if (result < 0) 
    {
      zip_source_free(source);
    }

}

void addFolderToZip(struct zip* archive, const char* folderPath, const char* baseDir)
{
    DIR* dir;
    struct dirent* entry;
    char filePath[MAX_SIZE];

    dir = opendir(folderPath);
    if (!dir) 
    {
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(filePath, sizeof(filePath), "%s/%s", folderPath, entry->d_name);

        struct stat fileStat;
        if (stat(filePath, &fileStat) == 0)
        {
            if (S_ISREG(fileStat.st_mode))
            {
                char entryName[MAX_SIZE];
                snprintf(entryName, sizeof(entryName), "%s%s%s", baseDir, (baseDir[strlen(baseDir) - 1] == '/') ? "" : "/", entry->d_name);
                addFileToZip(archive, filePath, entryName);
            }
            
            else if (S_ISDIR(fileStat.st_mode))
            {
                char subFolderBaseDir[MAX_SIZE];
                snprintf(subFolderBaseDir, sizeof(subFolderBaseDir), "%s%s%s", baseDir, (baseDir[strlen(baseDir) - 1] == '/') ? "" : "/", entry->d_name);
                addFolderToZip(archive, filePath, subFolderBaseDir);
            }
        }
    }

    closedir(dir);
}

int zipFolder(const char* folderPath, const char* zipFilePath) 
{
    struct zip* archive = zip_open(zipFilePath, ZIP_CREATE | ZIP_TRUNCATE, NULL);
    if (!archive) 
    {
        return 1;
    }

    addFolderToZip(archive, folderPath, "");
    if (zip_close(archive) < 0) 
    {
        return 1;
    }

    removeDirectory(folderPath);
    return 0;
}


//-----------------------------------
//UNZIP FUNCTION
//------------------------------------
int unzipFolder(const char* zipFilePath, const char* destinationFolder)
{
    struct zip* archive = zip_open(zipFilePath, 0, NULL);
    if (!archive)
    {
        return 1;
    }

    mkdir(destinationFolder, S_IRWXU | S_IRWXG | S_IRWXO);

    int numEntries = zip_get_num_entries(archive, 0);
    for (int i = 0; i < numEntries; i++)
    {
        struct zip_stat entryStat;
        if (zip_stat_index(archive, i, 0, &entryStat) == 0)
        {
            if (entryStat.name[strlen(entryStat.name) - 1] == '/')
            {
                // Skip directories
                continue;
            }

            char entryName[MAX_SIZE];
            snprintf(entryName, sizeof(entryName), "%s%s%s", destinationFolder, (destinationFolder[strlen(destinationFolder) - 1] == '/') ? "" : "/", entryStat.name);

            struct zip_file* file = zip_fopen_index(archive, i, 0);
            if (!file)
            {
                continue;
            }

            // Create directory if it doesn't exist
            char dirName[MAX_SIZE];
            strncpy(dirName, entryName, sizeof(dirName));
            char* lastSlash = strrchr(dirName, '/');
            if (lastSlash)
            {
                *lastSlash = '\0';
                mkdir(dirName, 0777);
            }

            FILE* outputFile = fopen(entryName, "wb");
            if (!outputFile)
            {
                zip_fclose(file);
                continue;
            }

            char buffer[4096];
            zip_int64_t bytesRead;
            while ((bytesRead = zip_fread(file, buffer, sizeof(buffer))) > 0)
            {
                fwrite(buffer, 1, bytesRead, outputFile);
            }

            fclose(outputFile);
            zip_fclose(file);

            // Set file permissions
            chmod(entryName,(mode_t)0644);
        }
    }

    zip_close(archive);
    remove(zipFilePath);

    return 0;
}

void removeDirectory(const char* path)
{
    DIR* directory = opendir(path);
    struct dirent* item;

    if (directory != NULL)
    {
        while ((item = readdir(directory)) != NULL)
        {
            if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0)
            {
                char itemPath[1024];
                snprintf(itemPath, sizeof(itemPath), "%s/%s", path, item->d_name);
                struct stat statbuf;
                if (stat(itemPath, &statbuf) == 0)
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        removeDirectory(itemPath);
                    }
                    
                    else
                    {
                        remove(itemPath);
                    }
                }
            }
        }
        closedir(directory);
    }

    rmdir(path);
}
