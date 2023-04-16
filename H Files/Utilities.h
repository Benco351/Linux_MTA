//Utilities file
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char** get_flights_data_in_airport(char* fileName);
int howManyRowsInFile(FILE* file);
void checkAllocation(void* pointer);
char** splitString(char* str);
void arrangeString(char** emptyString, char** dataString);