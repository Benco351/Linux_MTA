#include "Q1.h"

void printFlightsToAirport(char* airportName) //Prints all flights details to airportName.
{
    FILE* arrivalsFile, *departuresFile;
    openFilesByAirportName(airportName, &departuresFile, &arrivalsFile);
    checkAllocation(arrivalsFile);
    checkAllocation(departuresFile);
    int numOfFlights = howManyRowsInFile(arrivalsFile);
    char rows[MAX_SIZE];
    fgets(rows, MAX_SIZE, arrivalsFile); //'flush' first row.

    FlightData* flightsData = (FlightData*)malloc(sizeof(FlightData) * numOfFlights);
    checkAllocation(flightsData);

    for (int i = 0; i < numOfFlights; i++)
    {
        fgets(rows, MAX_SIZE, arrivalsFile);
        flightsData[i] = splitS(rows);
        printFlightsData(flightsData[i]);
    }

    free(flightsData);
    fclose(arrivalsFile);
    fclose(departuresFile);
}

void printFlightsData(FlightData object)
{
    printf("Flight #%s arriving from %s, tookoff at %s landed at %s",
    object.flightNumber, object.departureAirPort, object.firstSeen, object.lastSeen);
}