#!/bin/bash

gcc -c -fPIC Utilities.c Utilities.h
gcc -shared -o libUtilities.so Utilities.o
gcc -c arrivals.c Q1.c Q1.h
gcc -c full_schedule.c Q2.c Q2.h
gcc -c airplane.c Q3.c Q3.h
gcc -c RefreshDB.c
gcc -o my_program_1 arrivals.c Q1.o -L. -lUtilities
gcc -o my_program_2 full_schedule.c Q2.o -L. -lUtilities
gcc -o my_program_3 airplane.c Q3.o -L. -lUtilities
gcc -o my_program_4 RefreshDB.o -L. -lUtilities
