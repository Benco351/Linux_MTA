#include "Q1.h"

//this main gets list of airports
//prints all flights(arivals and departures) to said airports from data base
void main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        printFlightsToAirport(argv[i]);
    }
}