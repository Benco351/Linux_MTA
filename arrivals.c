#include "Q1.h"

void main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        printFlightsToAirport(argv[i]);
    }
}