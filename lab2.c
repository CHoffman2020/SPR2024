#define _GNU_SOURCE
#include <sys/time.h>
#include <sys/times.h>
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
#include <aio.h>

// cd ~/src/lab2
// gcc -g -pthread -o lab2 lab2.c

int SIZE = 10000000;
char ALT = 'S';		  // S
int THRESH = 10;	  // 10
int SEED = 0;		  // Random
char MULTHREAD = 'Y'; // Y
int PIECES = 10;	  // 10
int MAXTHREAD = 4;	  // 4
char MED3 = 'N';	  // N
char EARLY = 'Y';	  // Y
int *array;

double totalLoadTime, totalSortTime, totalTotalTime, totalCPUTime;
bool validStart = false, validEnd = false;

struct timeval loadTimeStart, loadTimeEnd;
struct timeval sortTimeStart, sortTimeEnd;
struct timeval totalTimeStart, totalTimeEnd;
struct tms CPU_TimeStart, CPU_TimeEnd;

typedef struct
{
	int start;
	int end;
	int *data;
}
_Thread;

//pthread_t *ID;
//_Thread *tArray;

#define FILENAME "random.dat"
// ADD RANDOM FILE ONCE U GET THE SORTING WORKING
bool isSorted(int *sArray, int Num);
void QuickSort(int *qarray, int lo, int hi);
void *Threading (void *ThreadArray);


int main(int argc, char *argv[], char *env[])
{
	while(!validStart)
	{
		if(gettimeofday(&totalTimeStart, NULL) != 0)
		{
			continue;
		}
		if(totalTimeStart.tv_sec > 0 || totalTimeStart.tv_usec > 0) {
            validStart = true;
        }
	}
	validStart = false;
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
			printf("\nNO INPUTS GIVEN");
			return 0;
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
	//clock_gettime(CLOCK_MONOTONIC, &totalTimeStart);
	//clock_gettime(CLOCK_MONOTONIC, &loadTimeStart);

	//gettimeofday(&totalTimeStart, NULL);

	while(!validStart)
	{
		if(gettimeofday(&loadTimeStart, NULL) != 0)
		{
			continue;
		}
		if(loadTimeStart.tv_sec > 0 || (loadTimeStart.tv_sec == 0 && loadTimeStart.tv_usec > 0)) {
            validStart = true;
        }
	}
	validStart = false;
	
	//gettimeofday(&loadTimeStart, NULL);

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
	//int x = (int)(loadTimeStart.tv_sec + loadTimeStart.tv_usec);
	//int y = (int)(loadTimeEnd.tv_sec + loadTimeEnd.tv_usec);
	//printf("Start: %d\nEnd: %d\n", x, y);
	fclose(inputFile);

	while(!validEnd)
	{
		if(gettimeofday(&loadTimeEnd, NULL) != 0)
		{
			continue;
		}
		if(loadTimeEnd.tv_sec > 0 || (loadTimeEnd.tv_sec == 0 && loadTimeEnd.tv_usec > 0)) {
            validEnd = true;
		}
	}
	validEnd = false;

	//gettimeofday(&loadTimeEnd, NULL);
	//clock_gettime(CLOCK_MONOTONIC, &loadTimeEnd);
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
	times(&CPU_TimeStart);
	//gettimeofday(&sortTimeStart, NULL);
	// MULTITHREADING WITH AND WITHOUT MEDIAN
	if (MULTHREAD == 'Y')
	{
		while(!validStart)
		{
			if(gettimeofday(&sortTimeStart, NULL) != 0)
			{
				continue;
			}
			if(sortTimeStart.tv_sec > 0 || (sortTimeStart.tv_sec == 0 && sortTimeStart.tv_usec > 0)) {
				validStart = true;
			}
		}
		printf("Creating multithread partitions\n");
		int start[PIECES];
		int end[PIECES];
		int pivot;
		//int piece = 1;
		//clock_gettime(CLOCK_MONOTONIC, &sortTimeStart);
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
		for(int i = 0; i < PIECES; i++)
		{
			int pieceSize = (end[i] - start[i]);
			double piecePerc = (pieceSize != 0) ? ((double)SIZE / pieceSize) : 0.0;
			printf("%d: %3d -  %3d", i, start[i], end[i]);
			printf(" ( %3d - %2.1f)\n", pieceSize, piecePerc);
		}
		//pthread_t *ID;
		//_Thread *tArray;
		pthread_t *ID = malloc(MAXTHREAD * sizeof(pthread_t));	//threads
		if (ID == NULL)
		{
			fprintf(stderr, "Array allocation failed\n");
			return 1; // Exit the program if memory allocation fails
		}
		_Thread *tArray = malloc(MAXTHREAD * sizeof(_Thread)); //Array
		if (tArray == NULL)
		{
			fprintf(stderr, "Array allocation failed\n");
			return 1; // Exit the program if memory allocation fails
		}
		int Counter = 0, sCounter = 0, done = 0, result = 0;
		
			if(MAXTHREAD > 0 && MAXTHREAD <= 4)
			{
				for(int i = 0; i < MAXTHREAD; i++)
				{
					tArray[i].end = end[i];
					tArray[i].start = start[i];
					pthread_create(&ID[i], NULL, Threading, (void *)&tArray[i]);
					printf("Launching thread to sort: %3d -  %3d ", tArray[i].start, tArray[i].end);
					printf("( %3d): row( %d)\n", tArray[i].end - tArray[i].start, sCounter);
					Counter++;
					sCounter++;
				}
			}
			else
			{
				printf("MAXTHREAD INPUT IS INVALID\n");
			}
			while(done < PIECES)
			{
				for(int i = 0; i < MAXTHREAD; i++)
				{
					result = pthread_tryjoin_np(ID[i], NULL);
					if(result == 0)
					{				
						done++;
						Counter++;
						if(sCounter < PIECES)
						{
							tArray[i].end = end[sCounter];
							tArray[i].start = start[sCounter];
							pthread_create(&ID[i], NULL, Threading, (void *)&tArray[i]);
							printf("Launching thread to sort: %3d -  %3d ", tArray[i].start, tArray[i].end);
							printf("( %3d): row( %d)\n", tArray[i].end - tArray[i].start, sCounter);
							Counter++;
							sCounter++;
						}
					}
					else if(done == EBUSY)
					{
						continue;
					}
				}
				usleep(100000);
			}
			free(tArray);
			free(ID);

			/*
			for (int i = 0; i < PIECES; i++)
			{
				QuickSort(array, start[i], end[i]);
				//printf("PARTION %d SORTED\n", i);
			}
			*/
			while(!validEnd)
			{
				if(gettimeofday(&sortTimeEnd, NULL) != 0)
				{
					continue;
				}
				if(sortTimeEnd.tv_sec > 0 || (sortTimeEnd.tv_sec == 0 && sortTimeEnd.tv_usec > 0)) {
					validEnd = true;
				}
			}
			validEnd = false;
			times(&CPU_TimeEnd);

	    //gettimeofday(&sortTimeEnd, NULL);

		//gettimeofday(&totalTimeEnd, NULL);
	}
	else
	{
		gettimeofday(&sortTimeStart, NULL);
		while(!validStart)
		{
			if(gettimeofday(&sortTimeStart, NULL) != 0)
			{
				continue;
			}
			if(sortTimeStart.tv_sec > 0 || (sortTimeStart.tv_sec == 0 && sortTimeStart.tv_usec > 0)) {
				validStart = true;
			}
		}
		validStart = false;
		times(&CPU_TimeStart);
		QuickSort(array, low, high); // QUICK SORTING
		while(!validEnd)
		{
			if(gettimeofday(&sortTimeEnd, NULL) != 0)
			{
				continue;
			}
			if(sortTimeEnd.tv_sec > 0 || (sortTimeEnd.tv_sec == 0 && sortTimeEnd.tv_usec > 0)) {
				validEnd = true;
			}
		}
		validEnd = false;
		//gettimeofday(&sortTimeEnd, NULL);
		//gettimeofday(&totalTimeEnd, NULL);
	}
	if(isSorted(array, SIZE) == false) 
	{
		perror("ERROR: List is not sorted\n");
	}
	free(array);
	times(&CPU_TimeEnd);
	while(!validEnd)
	{
		if(gettimeofday(&totalTimeEnd, NULL) != 0)
		{
			continue;
		}
		if(totalTimeEnd.tv_sec > 0 || (totalTimeEnd.tv_sec == 0 && totalTimeEnd.tv_usec > 0)) {
			validEnd = true;
		}
	}
		validEnd = false;
	
	printf("\n\nPOST SORTING\n");
	totalLoadTime = (double)(loadTimeEnd.tv_sec - loadTimeStart.tv_sec) + ((loadTimeEnd.tv_usec - loadTimeStart.tv_usec) / 1000000.0);
	totalSortTime = (double)(sortTimeEnd.tv_sec - sortTimeStart.tv_sec) + ((sortTimeEnd.tv_usec - sortTimeStart.tv_usec) / 1000000.0);
	totalCPUTime = (double)(CPU_TimeEnd.tms_utime + CPU_TimeEnd.tms_cstime - CPU_TimeStart.tms_utime - CPU_TimeStart.tms_stime)/ sysconf(_SC_CLK_TCK);
	totalTotalTime = (double)(totalTimeEnd.tv_sec - totalTimeStart.tv_sec) + ((totalTimeEnd.tv_usec - totalTimeStart.tv_usec) / 1000000.0);

	printf("Load: %0.3f  ", totalLoadTime);
	printf("Sort (Wall/CPU):   %0.3f /   %0.3f  ", totalSortTime, totalCPUTime);
	printf("Total: %0.3f\n", totalTotalTime);
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
	for(int i = 0; i < Num - 1; i++)
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

void *Threading(void *ThreadArray)
{
	_Thread *tThreading = (_Thread *)ThreadArray;
	int high = tThreading->end;
	int low = tThreading->start;
	QuickSort(array, low, high);
	pthread_exit(NULL);
}
