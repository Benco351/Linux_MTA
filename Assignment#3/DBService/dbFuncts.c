#include "Utilities.h"

//Builds DB data structure.
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
        if (departuresFile == NULL || arrivalsFile == NULL)
        {
            free(DataBase);
            DataBase = NULL;
            return DataBase;
        }

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

//Sorts the array of airports names.
char** reorderStringArray(int numOfArgs, char* airports[])
{
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

//Loads database according to arguments.
void loadDatabase(int numOfArgs, char* airports[])
{
    int result;
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

    result = system(commandLine);
    system("rm -rf ../flightsDB/");
    system("mv flightsDB ..");
}

//Returns all flights details of airportName.
char** printFlightsToAirport(char* airportName, DB* db, int* logSize ,int mission)
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
        if (mission == INCOMING_FLIGHTS)
        {
            for (int i = 0; i < db->airPortsArr[j].SizeArrivals; i++) //for each arrival
            {
                if (*logSize == phySize)
                {
                    phySize *= 2;
                    output = (char**)realloc(output, sizeof(char*) * phySize);
                    checkAllocation(output);
                }
                output[*logSize] = printFlightsData(db->airPortsArr[j].arrivals[i], INCOMING_FLIGHTS);
                (*logSize)++;
            }
        }

        else if (mission == FULL_SCHEDULE)
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
        currStringSize = snprintf(NULL, 0, "%s does not exist in data base.\n", airportName);
        output[*logSize] = (char*)malloc(currStringSize + 1);
        checkAllocation(output[*logSize]);
        snprintf(output[*logSize], currStringSize + 1, "%s does not exist in data base.\n", airportName);
        (*logSize)++;
    }

    if (phySize > (*logSize))
    {
        output = (char**)realloc(output, sizeof(char*) * (*logSize));
        checkAllocation(output);
    }

    return output;
}

//Returns a string of flights data.
char* printFlightsData(FlightData object, int mission)
{
    char* output = (char*)malloc(sizeof(char) * MAX_SIZE);
    checkAllocation(output);
    int logSize = 0;

    switch (mission)
    {
        case INCOMING_FLIGHTS:
        {
            logSize = snprintf(NULL, 0, "Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
                               object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
            snprintf(output, logSize + 1, "Flight #%-7s arriving from %s, tookoff at %s landed at %s\n",
                     object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
        }
        case FULL_SCHEDULE:
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
        case AIRCRAFT_SCHEDULE:
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

//Runs the request on the DB and writes the result to the FIFO.
void runRequestOnDB(char* parameters[], int numOfParameters, DB* db, int FIFO, int mission)
{
    char** buffer = NULL, **output = NULL;
    int bufferLogSize = 0, bufferPhySize = MAX_SIZE, outputLogSize = 0, bytesCounter = 0;
    buffer = (char**)malloc(sizeof(char*) * MAX_SIZE);
    checkAllocation(buffer);

    if (mission == AIRCRAFT_SCHEDULE)
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

    bytesCounter = write(FIFO, &bufferLogSize, sizeof(int));
    while (bytesCounter != sizeof(int))
    {
        bytesCounter += write(FIFO, &bufferLogSize, sizeof(int));
    }
    ReadOrWriteToPipe(buffer, bufferLogSize, FIFO, WRITE);

    for (int i = 0; i < bufferLogSize; i++)
    {
        free(buffer[i]);
    }

    free(buffer);
}

//Returns an airport's time-sorted flights data.
char* compareFlights(DB* db, int *a, int *d, int airport)
{
    if (*a == db->airPortsArr[airport].SizeArrivals)
    {
        (*d) +=1;
        return printFlightsData(db->airPortsArr[airport].Departures[(*d)-1], FULL_SCHEDULE);
    }
    if (*d == db->airPortsArr[airport].SizeArrivals)
    {
        (*a) +=1;
        return printFlightsData(db->airPortsArr[airport].arrivals[(*a)-1], FULL_SCHEDULE);
    }
    if (strcmp(db->airPortsArr[airport].arrivals[(*a)].lastSeen, db->airPortsArr[airport].Departures[(*d)].firstSeen) < 0)
    {
        (*d) +=1;
        return printFlightsData(db->airPortsArr[airport].Departures[(*d)-1], FULL_SCHEDULE);
    }
    else
    {
        (*a) +=1;
        return printFlightsData(db->airPortsArr[airport].arrivals[(*a) -1], FULL_SCHEDULE);
    }
}

//Returns aircraft's data.
void findAirCrafts(char** aircraft, int nofAirCrafts, DB* db, char*** output, int* logSize, int* phySize)
{
    int currentStringSize = 0;
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
                    (*output)[*logSize] = printFlightsData(db->airPortsArr[j].arrivals[k], AIRCRAFT_SCHEDULE);
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
                    (*output)[*logSize] = printFlightsData(db->airPortsArr[j].Departures[k], AIRCRAFT_SCHEDULE);
                    (*logSize)++;
                }
            }
        }
    }

    if ((*logSize) == 0)
    {
        currentStringSize = snprintf(NULL, 0, "%s does not exist in data base.\n", aircraft[0]);
        *(output[*logSize]) = (char*)malloc(currentStringSize + 1);
        checkAllocation(*(output[*logSize]));
        snprintf(*(output[*logSize]), currentStringSize + 1, "%s does not exist in data base.\n", aircraft[0]);
        (*logSize)++;
    }
}

//Re-runs the script and creates a new DB.
DB* reRunScript(char** inputArr, int arrSize, DB* dataBase, int FIFO)
{
    DB* output = NULL;
    int BytesCounter = 0, stringSize = 0;
    loadDatabase(arrSize, inputArr);
    output = getDataBase(arrSize, inputArr);

    return output;
}

//Creates a FlightData data structure from a string.
FlightData splitS(char* str)
{
    FlightData flightData;
    strcpy(flightData.icao24, strtok(str, ","));
    strcpy(flightData.firstSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(flightData.departureAirPort, strtok(NULL, ","));
    strcpy(flightData.lastSeen, unix_time_to_date(strtok(NULL, ",")));
    strcpy(flightData.arrivalAirPort, strtok(NULL, ","));
    strcpy(flightData.flightNumber, strtok(NULL, ",\n\r"));

    return flightData;
}

//Free the DB data structure.
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