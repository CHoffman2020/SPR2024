#define _GNU_SOURCE
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

// cd ~/src/lab2
// gcc -g -pthread -o lab2 lab2.c

int SIZE = 15;
char ALT = 'S';		  // S
int THRESH = 10;	  // 10
int SEED = 0;		  // Random
char MULTHREAD = 'Y'; // Y
int PIECES = 4;	  // 10
int MAXTHREAD = 4;	  // 4
char MED3 = 'N';	  // N
char EARLY = 'N';	  // Y
int *array;

#define FILENAME "random.dat"
// ADD RANDOM FILE ONCE U GET THE SORTING WORKING
bool isSorted(int *sArray, int Num);
void QuickSort(int *qarray, int lo, int hi);

int main(int argc, char *argv[], char *env[])
{
	for (int i = 1; i < argc; i += 2)
	{
		if (strcmp(argv[i], "-n") == 0)
		{
			SIZE = atoi(argv[i + 1]);
			printf("SIZE: %d\n", SIZE);
		}
		else if (strcmp(argv[i], "-a") == 0)
		{
			// printf("%s\n", argv[i]);
			if ((strcmp(argv[i + 1], "I") == 0) || (strcmp(argv[i + 1], "i") == 0))
			{
				ALT = 'I';
			}
			else
			{
				ALT = 'S';
			}
			printf("ALTERNATE: %c\n", ALT);
		}
		else if (strcmp(argv[i], "-s") == -0)
		{
			// printf("%s\n", argv[i]);
			THRESH = atoi(argv[i + 1]);
			printf("THRESHOLD: %d\n", THRESH);
		}
		else if (strcmp(argv[i], "-r") == 0)
		{
			// printf("%s\n", argv[i]);
			SEED = atoi(argv[i + 1]);
			printf("SEED: %d\n", SEED);
		}
		else if (strcmp(argv[i], "-m") == 0)
		{
			// printf("%s\n", argv[i]);
			if ((strcmp(argv[i + 1], "N") == 0) || (strcmp(argv[i + 1], "n") == 0))
			{
				MULTHREAD = 'N';
			}
			else
			{
				MULTHREAD = 'Y';
			}
			printf("MULTITHREAD: %c\n", MULTHREAD);
		}
		else if (strcmp(argv[i], "-p") == 0)
		{
			// printf("%s\n", argv[i]);
			PIECES = atoi(argv[i + 1]);
			printf("PIECES: %d\n", PIECES);
		}
		else if (strcmp(argv[i], "-t") == 0)
		{
			// printf("%s\n", argv[i]);
			MAXTHREAD = atoi(argv[i + 1]);
			printf("MAXTHREAD: %d\n", MAXTHREAD);
		}
		else if (strcmp(argv[i], "-m3") == 0)
		{
			// printf("%s\n", argv[i]);
			if ((strcmp(argv[i + 1], "Y") == 0) || (strcmp(argv[i + 1], "y") == 0))
			{
				MED3 = 'Y';
			}
			else
			{
				MED3 = 'N';
			}
			printf("MEDIAN: %c\n", MED3);
		}
		else if (strcmp(argv[i], "-e") == 0)
		{
			// printf("%s\n", argv[i]);
			if ((strcmp(argv[i + 1], "Y") == 0) || (strcmp(argv[i + 1], "y") == 0))
			{
				EARLY = 'Y';
			}
			else
			{
				EARLY = 'N';
			}
			printf("EARLY: %c\n", EARLY);
		}
		else
		{
			printf("\nERROR");
		}
	}
	printf("\n");
	if(SEED == 0)
	{
		SEED = rand() % 1000000000;
	}
	srand(time(NULL));
	// GRABBING A SPECIFIC CHUNK OF THE MEMORY IN THE FILE AND CONVERTING THROWING THEM INTO AN array
	// ALSO HANDLED STARTING AT THE END OF A FILE AND WRAPPING BACK AROUND TO THE FRONT
	array = (int *)malloc(SIZE * sizeof(int));
	if (array == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		return 1; // Exit the program if memory allocation fails
	}

	FILE *inputFile = fopen(FILENAME, "rb");
	if (!inputFile)
	{
		printf("Failed to open %s File \n", FILENAME);
	}
	fseek(inputFile, SEED * sizeof(int), SEEK_SET);
	int dataCount = fread(array, sizeof(int), SIZE, inputFile);
	if(dataCount < SIZE)
	{
		fseek(inputFile, 0, SEEK_SET);
		int leftOverData = SIZE - dataCount;
		fread(array + dataCount, sizeof(int), leftOverData, inputFile);
	}

	fclose(inputFile);
	/*
	for(int i=0; i < SIZE; i++)
	{
		printf("%d\n", array[i]);
	}
	*/
	int low = 0;
	int high = SIZE - 1;
	// MEDIAN FOR THE NORMAL array ITSELF BEFORE PARTITIONING
	if (MED3 == 'Y' && MULTHREAD == 'N')
	{
		int mid = low + (high - low) / 2;
		printf("MEDIAN USED  \n");
		if (array[low] > array[mid])
		{
			int temp = array[low];
			array[low] = array[mid];
			array[mid] = temp;
		}
		if (array[mid] > array[high])
		{
			int temp = array[high];
			array[high] = array[mid];
			array[mid] = temp;
			if (array[low] > array[mid])
			{
				int temp = array[low];
				array[low] = array[mid];
				array[mid] = temp;
			}
		}
	}

	// MULTITHREADING WITH AND WITHOUT MEDIAN
	if (MULTHREAD == 'Y')
	{
		printf("Creating multithread partitions\n");
		int start[PIECES];
		int end[PIECES];
		int pivot;
		//int piece = 1;
		for(int piece = 1; piece < PIECES; piece++)
		{
			if(piece == 1)
			{
				start[0] = 0;
				end[0] = SIZE - 1;
			}
			low = start[0];
			high = end[0];
			//int max = 0;
			printf("Partioning %3d   -   %3d (%d)...", low, high, high - low);
			if (MED3 == 'Y')
			{
				int mid = low + (high - low) / 2;
				//printf("MEDIAN USED  \n");
				if (array[low] > array[mid])
				{
					int temp = array[low];
					array[low] = array[mid];
					array[mid] = temp;
				}
				if (array[mid] > array[high])
				{
					int temp = array[high];
					array[high] = array[mid];
					array[mid] = temp;
					if (array[low] > array[mid])
					{
						int temp = array[low];
						array[low] = array[mid];
						array[mid] = temp;
					}
				}
			}
			pivot = array[low];
			int i = low;
			int j = high + 1;
			do
			{
				do
					i++;
				while (array[i] < pivot); // advance forward in the array while the pivot is greater than the value
				do
					j--;
				while (array[j] > pivot); // advance backward in the array while the pivot is less than the value
				if (i < j)
				{ // if pointers cross swap them.
					int temp = array[i];
					array[i] = array[j];
					array[j] = temp;
				}
				else
					break; // else partitioning is finished
			} while (1);
			int temp = array[low];
			array[low] = array[j];
			array[j] = temp;


			double RPerc = (high - low + 1 > 0) ? ((double)(high - j - 1) / (double)(high - low + 1)) * 100 : 0.0;
			double LPerc = (high - low + 1 > 0) ? ((double)(j - 1 - low) / (double)(high - low + 1)) * 100 : 0.0;
			printf("results: %3d   -  %3d (%2.1f / %2.1f)\n", j - 1 - low, high - j + 1, LPerc, RPerc);

			start[0] = low;
			end[0] = j - 1;
			start[piece] = j + 1;
			end[piece] = high;

			for(int first = 0; first < piece; first++)
			{
				for(int second = first + 1; second < piece + 1; second++)
				{
					if(end[first] - start[first] < end[second] - start[second])
					{
						int temp = start[first];
						start[first] = start[second];
						start[second] = temp;

						temp = end[first];
						end[first] = end[second];
						end[second] = temp;
					}
				}
			}
        }
		/*
		for (int i = 0; i < PIECES; i++)
		{
			printf("PIECE: %d  |   %d  -  %d\n", i, start[i], end[i]);
		}
		
		for(int i=0; i < SIZE; i++)
		{
			printf("%d\n", array[i]);
		}
		*/
		for (int i = 0; i < PIECES; i++)
		{
			QuickSort(array, start[i], end[i]);
			//printf("PARTION %d SORTED\n", i);
		}
	}
	// NON MULTITHREADING
	else
	{
		QuickSort(array, low, high); // QUICK SORTING
		//printf("NO MULTITHREAD\n\n");
	}
	//if(!isSorted(array, SIZE)) perror("ERROR: List not sorted\n");
	printf("\n\nPOST SORTING\n");

	/*
	for(int i=0; i < SIZE; i++)
	{
		printf("%d\n", array[i]);
	}
	*/
	return 0;

	// Time how long it takes to read in values with 0.001 decimal format
	// Record the timings (&array[#] to grab the value in the index)
}

bool isSorted(int sArray[], int Num)
{
	for(int i = 0; i < Num; i++)
	{
		if(sArray[i] > sArray[i + 1])
		{
			return false;
		}
	}
	return true;
}

void QuickSort(int qarray[], int lo, int hi)
{
	int segmentSize = hi - lo + 1;
	// Check the size of the segment and apply the rules
	if (segmentSize < 2)
	{
		// Do nothing - segment is empty or has a single value
		return;
	}
	else if (segmentSize == 2)
	{
		// Compare and swap if necessary
		if (qarray[lo] > qarray[hi])
		{
			int temp = qarray[lo];
			qarray[lo] = qarray[hi];
			qarray[hi] = temp;
		}
		return;
	}
	else if (segmentSize == 3)
	{
		// Use the comparison tree to determine order
		if (qarray[lo] > qarray[hi])
		{
			int temp = qarray[lo];
			qarray[lo] = qarray[hi];
			qarray[hi] = temp;
		}
		if (qarray[lo] > qarray[hi - 1])
		{
			int temp = qarray[lo];
			qarray[lo] = qarray[hi - 1];
			qarray[hi - 1] = temp;
		}
		if (qarray[hi - 1] > qarray[hi])
		{
			int temp = qarray[hi - 1];
			qarray[hi - 1] = qarray[hi];
			qarray[hi] = temp;
		}
		return;
		// Check we haven't partitioned down to a level the user wants to use a simpler sorting method
	}
	else if (segmentSize <= THRESH)
	{
		if (ALT == 'S')
		{
			int k = segmentSize;
    		while (k > 0) 
			{
       		 	for (int i = lo; i < hi - k + 1; i++) 
			 	{
           	 		for (int j = i; j >= lo && qarray[j] > qarray[j + k]; j -= k) 
					{
                		int temp = qarray[j];
                		qarray[j] = qarray[j + k];
                		qarray[j + k] = temp;
            		}
        		}
        	k /= 2;
    		}
		}
		// ALTERNATIVE SORT METHOD S for SHELL... I FOR INSERT
		if (ALT == 'I')
		{
			for (int i = lo + 1; i <= hi; ++i)
			{
				int keyVal = qarray[i];
				int j = i - 1;
				while (j >= lo && qarray[j] > keyVal)
				{
					qarray[j + 1] = qarray[j];
					--j;
				}
				qarray[j + 1] = keyVal;
			}
		}
	}
	else
	{
		int pivot = qarray[lo];
		int i = lo;		// Set front index
		int j = hi + 1; // Set back index
		do
		{
			do
				i++;
			while (qarray[i] < pivot); // advance forward in the array while the pivot is greater than the value
			do
				j--;
			while (qarray[j] > pivot); // advance backward in the array while the pivot is less than the value
			if (i < j)
			{ // if pointers cross swap them.
				int temp = qarray[i];
				qarray[i] = qarray[j];
				qarray[j] = temp;
			}
			else
				break; // else partitioning is finished
		} while (1);
		int temp = qarray[lo];
		qarray[lo] = qarray[j];
		qarray[j] = temp;
		if (lo - j - 1 < hi - j + 1)
			QuickSort(qarray, lo, j - 1);

		QuickSort(qarray, j + 1, hi);
	}
}
