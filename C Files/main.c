#include "Utilities.h"

int main()
{
    char** data = get_flights_data_in_airport("test.csv");
    for (int i = 0; i < 2; i++)
        printf("%s\nֿֿ", data[i]);

    return 1;
}