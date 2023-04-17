#include "Q2.h"

void printAirportSchedule(char* airportName)
{
    FILE* arrivalsFile, *departuresFile;
    openFilesByAirportName(airportName, &departuresFile, &arrivalsFile);
    checkAllocation(arrivalsFile);
    checkAllocation(departuresFile);

    int numOfArrivals = howManyRowsInFile(arrivalsFile);
    int numOfDepartures = howManyRowsInFile(departuresFile);

    char arrivals[MAX_SIZE];
    char departures[MAX_SIZE];

    fgets(arrivals, MAX_SIZE, arrivalsFile);
    fgets(departures, MAX_SIZE, departuresFile);

    FlightData* data = (FlightData*)malloc(sizeof(FlightData) * (numOfArrivals + numOfDepartures));
    checkAllocation(data);

    for (int i = 0; i < numOfArrivals; i++)
    {
        fgets(arrivals, MAX_SIZE, arrivalsFile);
        data[i] = splitS(arrivals);
        data[i].arrivalOrDeparture = ARRIVAL;
    }

    for (int i = numOfArrivals - 1; i < (numOfDepartures+numOfArrivals); i++)
    {
        fget(departures, MAX_SIZE, departuresFile);
        data[i] = splitS(departures);
        data[i].arrivalOrDeparture = DEPARTURE;
    }

    qsort(data, numOfDepartures + numOfArrivals - 1, sizeof(FlightData), compareFlights);
    
    for (int i = 0; i < numOfDepartures + numOfArrivals; i++)
    {
        printFullSchedule(data[i]);
    }

    free(data);
    fclose(arrivalsFile);
    fclose(departuresFile);
}

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

void printFullSchedule(FlightData object)
{
    if (object.arrivalOrDeparture == ARRIVAL)
    {
        printf("Flight #%s arriving from %s at %s", object.flightNumber, object.departureAirPort, object.lastSeen);
    }

    else if (object.arrivalOrDeparture == DEPARTURE)
    {
        printf("Flight #%s departing to %s at %s", object.flightNumber, object.arrivalAirPort, object.firstSeen);
    }
}