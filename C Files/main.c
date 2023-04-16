#include "Utilities.h"

int main()
{
    char** data = get_flights_data_in_airport("/home/eliormigdal/testing/test.csv");
    for (int i = 0; i < 2; i++)
        printf("%s\nֿֿ", data[i]);
}
