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

int SIZE = 1000000;
char ALT ='S';	//S
int THRESH = 10;	//10
int SEED = 0;		//Random
char MULTHREAD = 'Y'; //Y
int PIECES = 10;  //10
int MAXTHREAD = 4;  //4
char MED3 = 'Y';	//N
char EARLY = 'Y';  	//Y
int *array;

#define FILENAME "random.dat"
//ADD RANDOM FILE ONCE U GET THE SORTING WORKING
void QuickSort(int *qarray,int lo, int hi);

int main(int argc, char *argv[], char *env[])
{	
	for(int i=1; i < argc; i+=2)
	{
		if(strcmp(argv[i], "-n") == 0)
		{
			SIZE = atoi(argv[i + 1]);
			printf("%d\n", SIZE);
		}
		else if(strcmp(argv[i], "-a") == 0)
		{
			//printf("%s\n", argv[i]);
			if((strcmp(argv[i + 1], "I") == 0) || (strcmp(argv[i + 1], "i") == 0))
			{
				ALT = 'I';
			}
			else
			{
				ALT = 'S';
			}
			printf("%c\n", ALT);
		}
		else if(strcmp(argv[i], "-s") ==- 0)
		{
			//printf("%s\n", argv[i]);
			THRESH = atoi(argv[i + 1]);
			printf("%d\n", THRESH);
		}
		else if(strcmp(argv[i], "-r") == 0)
		{
			//printf("%s\n", argv[i]);
			SEED = atoi(argv[i + 1]);
			printf("%d\n", SEED);
		}
		else if(strcmp(argv[i], "-m") == 0)
		{
			//printf("%s\n", argv[i]);
			if((strcmp(argv[i + 1], "N") == 0) || (strcmp(argv[i + 1], "n") == 0))
			{
				MULTHREAD = 'N';
			}
			else
			{
				MULTHREAD = 'Y';
			}
			printf("%c\n", MULTHREAD);
		}		
		else if(strcmp(argv[i], "-p") == 0)
		{
			//printf("%s\n", argv[i]);
			PIECES = atoi(argv[i + 1]);
			printf("%d\n", PIECES);
		}
		else if(strcmp(argv[i], "-t") == 0)
		{
			//printf("%s\n", argv[i]);
			MAXTHREAD = atoi(argv[i + 1]);
			printf("%d\n", MAXTHREAD);
		}
		else if(strcmp(argv[i], "-m3") == 0)
		{
			//printf("%s\n", argv[i]);
			if((strcmp(argv[i + 1], "Y") == 0) || (strcmp(argv[i + 1], "y") == 0))
			{
				MED3 = 'Y';
			}
			else
			{
				MED3 = 'N';
			}
			printf("%c\n", MED3);
		}
		else if(strcmp(argv[i], "-e") == 0)
		{
			//printf("%s\n", argv[i]);
			if((strcmp(argv[i + 1], "Y") == 0) || (strcmp(argv[i + 1], "y") == 0))
			{
				EARLY = 'Y';
			}
			else
			{
				EARLY = 'N';
			}
			printf("%c\n", EARLY);
		}
		else
		{
			printf("\nERROR");
		}
	}

	//GRABBING A SPECIFIC CHUNK OF THE MEMORY IN THE FILE AND CONVERTING THROWING THEM INTO AN array
	//ALSO HANDLED STARTING AT THE END OF A FILE AND WRAPPING BACK AROUND TO THE FRONT
	array = (int *)malloc(SIZE * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Exit the program if memory allocation fails
    }
	FILE* inputFile = fopen(FILENAME, "rb");
	if (!inputFile)
	{
		printf("Failed to open %s File \n", FILENAME);
	}
	int farray[SIZE];
	srand(time(NULL));

	fseek(inputFile, 0, SEEK_END);
	long fileSize = ftell(inputFile);

	long file_notByte = fileSize/sizeof(int);
	
	if(SEED == 0)
	{
		SEED = rand() % file_notByte;
	}

	int high = SIZE - 1;
	int low = 0;
	bool needTwoarray = false;
	long start_offset = SEED * sizeof(int);
	fseek(inputFile, start_offset, SEEK_SET);
	long current_position = ftell(inputFile);
	long size_length = SIZE * sizeof(int);
    long end_offset = (SIZE + SEED) * sizeof(int);
	long file_Diff = fileSize - current_position;
	long file_nonByteDiff = file_Diff / sizeof(int);
	long file_BegDiff = SIZE - file_nonByteDiff;
	
	if(fseek(inputFile, start_offset, SEEK_SET) != 0)
	{
		perror("Error seeking file");
		fclose(inputFile);
		return 1;
	}
	if((file_Diff) < size_length)
	{	
		needTwoarray = true;
		fread(array, sizeof(int), file_nonByteDiff, inputFile);
		fseek(inputFile, 0, SEEK_SET);
		fread(farray, sizeof(int), file_BegDiff, inputFile);
		printf("END OF FILE -> BEGINNING NEEDED\n");
	}
	else
	{
		fread(array, sizeof(int), SIZE, inputFile);
	}

	fclose(inputFile);
	int iterate = 0;
	printf("FILESIZE:  %ld\n", fileSize);
	printf("START SEED:  %d\n", SEED);
	if(needTwoarray == true)
	{
		for(int i = file_nonByteDiff; i < SIZE; i++)
		{
		array[i] = farray[iterate];
		iterate++;
		}
		printf("FIRST array END:  %d\n", array[file_nonByteDiff - 1]);
		printf("SECOND array BEG:  %d\n", array[file_nonByteDiff]);
	}

	/*
	for(int i=0; i < SIZE; i++)
	{
		printf("%d\n", array[i]);
	}
	*/

	//MEDIAN FOR THE NORMAL array ITSELF BEFORE PARTITIONING
	if(MED3 == 'Y' && MULTHREAD == 'N')
	{
			int mid = low + (high - low) / 2;
			printf("MEDIAN USED  \n");
			if(array[low] > array[mid])
			{
				    int temp = array[low]; 
					array[low] = array[mid]; 
					array[mid] = temp;
			}
			if(array[mid] > array[high])
			{
				int temp = array[high]; 
				array[high] = array[mid]; 
				array[mid] = temp;
				if(array[low] > array[mid])
				{
					int temp = array[low]; 
					array[low] = array[mid]; 
					array[mid] = temp;
				}
			}
		low = mid;
	}

	//MULTITHREADING WITH AND WITHOUT MEDIAN
	if(MULTHREAD == 'Y')
	{
		int start[PIECES];
		int end[PIECES];
		int mid = 0;
		int max;
		int block_max = 0;
		for(int piece = 1; piece < PIECES; piece++)
		{

			if(MED3 == 'Y')
			{
				mid = low + (high - low) / 2;
				printf("MEDIAN USED  \n");
				if(array[low] > array[mid])
				{
						int temp = array[low]; 
						array[low] = array[mid]; 
						array[mid] = temp;
				}
				if(array[mid] > array[high])
				{
					int temp = array[high]; 
					array[high] = array[mid]; 
					array[mid] = temp;
					if(array[low] > array[mid])
					{
						int temp = array[low]; 
						array[low] = array[mid]; 
						array[mid] = temp;
					}
				}
			}
			int pivot = array[mid];
			int i = low;
			int j = high + 1;
			do {
				do i++; while (array[i] < pivot); //advance forward in the array while the pivot is greater than the value
				do j--; while (array[j] > pivot);//advance backward in the array while the pivot is less than the value
					if (i < j) 
					{    //if pointers cross swap them.
						int temp = array[i]; 
						array[i] = array[j]; 
						array[j] = temp;
					}
					else break;     //else partitioning is finished
			} while (1);
			int leftCount = i - low;
			int rightCount = high - i + 1;
			if(piece == 1)
			{
				start[piece - 1] = low;
				end[piece - 1] = i - 1;
				start[piece] = i + 1;
				end[piece] = high;
				if(leftCount < rightCount)
				{
					block_max = 1;
				}
			}
			else if (high != SIZE - 1)
			{
				end[block_max] = i - 1;
				start[piece] = i + 1;	
				end[piece] = high;
			}
			else
			{
				start[block_max] = i + 1;
				start[piece] = low;
				end[piece] = i - 1;
			}
			for(int aSize = 0; aSize < piece; aSize++)
			{
				if(end[aSize] - start[aSize] > max)
				{
					max = end[aSize] - start[aSize];
					block_max = aSize;
					if(piece == 1)
					{
						if(end[aSize + 1] - start[aSize + 1] > max)
						{
							max = end[aSize + 1] - start[aSize + 1];
							block_max = aSize + 1;
						}
					}
				}
			}
			low = start[block_max];
			high = end[block_max];
		}
		for(int i = 0; i < PIECES; i++)
		{
			printf("PIECE: %d  |   %d  -  %d\n", i, start[i], end[i]);
		}
		/*
		for(int i=0; i < SIZE; i++)
		{
			printf("%d\n", array[i]);
		}
		*/
		for(int i = 0; i < PIECES; i++)
		{
			QuickSort(array, start[i], end[i]);
			printf("PARTION %d SORTED\n", i);
		}
	}
	//NON MULTITHREADING
	else
	{
	QuickSort(array, low, high);			//QUICK SORTING
	printf("NO MULTITHREAD\n\n");
	}
	printf("\n\nPOST SORTING\n");
	
	/*
	for(int i=0; i < SIZE; i++)
	{
		printf("%d\n", array[i]);
	}
	*/
	return 0;

	//Time how long it takes to read in values with 0.001 decimal format
	//Record the timings (&array[#] to grab the value in the index)
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
        else if (segmentSize == 2) {
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
				qarray[lo] = qarray[hi-1]; 
				qarray[hi-1] = temp;
			}
            if (qarray[hi - 1] > qarray[hi])
			{ 
				int temp = qarray[hi-1]; 
				qarray[hi-1] = qarray[hi]; 
				qarray[hi] = temp;
			}
            return;
        //Check we haven't partitioned down to a level the user wants to use a simpler sorting method
        } 
        else if (segmentSize <= THRESH)
		{
			if(ALT == 'S')
			{
				int k = 1; while (k <= SIZE) k *= 2; k = ((k / 2) - 1);
				do 
				{
					for (int i = 0; i < ( SIZE - k); i++) // for each comb position
					{
						for (int j = i; j >= 0; j -= k) // Tooth-to-tooth is k
						{
							if (qarray[j] <= qarray[j + k]) 
							{
								break; // move upstream/exit?
							}
							else
							{
								int temp = qarray[j]; 
								qarray[j] = qarray[j + k]; //swap qarray[j] and qarray[j + k];
								qarray[j + k] = temp;
							}
						}
					}
					k /= 2; // or k /= 2;
				}
				while (k > 0 );
			}
			//ALTERNATIVE SORT METHOD S for SHELL... I FOR INSERT
			if(ALT == 'I')
			{
				for(int i=1; i < SIZE; i++)
				{
					int keyVal = qarray[i];
					int j = i-1;
					while(j >= 0 && qarray[j] > keyVal)
					{
						qarray[j+1] = qarray[j];
						j--;
					}
					qarray[j+1] = keyVal;
				}
			}
		}
	else{
		int pivot = qarray[lo];
        int i = lo;    //Set front index
        int j = hi + 1;    //Set back index
        do {
            do i++; while (qarray[i] < pivot); //advance forward in the array while the pivot is greater than the value
            do j--; while (qarray[j] > pivot);//advance backward in the array while the pivot is less than the value
                if (i < j) 
				{    //if pointers cross swap them.
            		int temp = qarray[i]; 
					qarray[i] = qarray[j]; 
					qarray[j] = temp;
                }
                else break;     //else partitioning is finished
        } while (1);
        int temp = qarray[lo]; 
		qarray[lo] = qarray[j]; 
		qarray[j] = temp;
		if(lo - j - 1 < hi - j + 1)
			QuickSort(qarray, lo, j - 1);

        QuickSort(qarray, j + 1, hi);
		}
}
