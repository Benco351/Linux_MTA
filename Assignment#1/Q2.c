#include "Q2.h"

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