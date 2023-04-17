#include "Q3.h"

//this function gets and open file of specific airport, the aircrafts searched for and their number
//returns all flights for said aircrafts in given airport
void findAirCrafts(FILE* f, char** aircrafts, int nofAirCrafts)
{
    int numOfFlights = howManyRowsInFile(f);
    char temp[MAX_SIZE];
    fgets(temp, MAX_SIZE, f); // get rid of info line

    for (int i = 0; i < numOfFlights; i++)
    {
        fgets(temp, MAX_SIZE, f);
        FlightData FD = splitS(temp);
        for (int j = 0; j < nofAirCrafts; j++)
        {
            if (strcmp(FD.icao24, aircrafts[j]))
            {
                printQ3(FD);
            }
        }
    }
}

void printQ3(FlightData FD)
{
    printf("%s deprted from %s at %s arrived in %s at %s\n", &FD.flightNumber, &FD.departureAirPort, &FD.firstSeen, &FD.arrivalAirPort, &FD.lastSeen);
}