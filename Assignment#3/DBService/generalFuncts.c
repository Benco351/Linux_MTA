#include "Utilities.h"

//Comparison function for qsort.
int compareStrings(const void* a, const void* b)
{
    const char** str1 = (const char**)a;
    const char** str2 = (const char**)b;
    return strcmp(*str1, *str2);
}

//Binary search function to find the needed airport.
int quickSearch(char* arr[], int size, char* target)
{
    int left = 0;
    int right = size - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;

        if (strcmp(arr[mid], target) == 0)
        {
            return mid;  // Found the target at index mid
        }

        if (strcmp(arr[mid], target) < 0)
        {
            left = mid + 1;  // Target is in the right half
        }

        else
        {
            right = mid - 1;  // Target is in the left half
        }
    }

    return -1;  // Target not found
}

//Verifies memory allocation.
void checkAllocation(void* pointer)
{
    if (pointer == NULL)
    {
        printf("Memory allocation failed. Exiting program...\n");
        exit(-1);
    }
}