#include "Q3.h"

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