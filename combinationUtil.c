// Program to print all combination of size r in an array of size n
#include <stdio.h>
#include <string.h>
#include "combinationUtil.h"
#include "permute.h"

// The main function that prints all combinations of size r
// in arr[] of size n. This function mainly uses combinationUtil()
void checkCombination(char arr[], int n, int r, void (*check)(BYTE *))
{
	// A temporary array to store all combination one by one
	char data[r+1];
	data[r] = 0;

	// Print all combination using temprary array 'data[]'
	combinationUtil(arr, data, 0, n-1, 0, r, check);
}

/* arr[] ---> Input Array
data[] ---> Temporary array to store current combination
start & end ---> Staring and Ending indexes in arr[]
index ---> Current index in data[]
r ---> Size of a combination to be printed */
void combinationUtil(char arr[], char data[], int start, int end,
					int index, int r, void (*check)(BYTE *))
{
	// Current combination is ready to be printed, print it
	if (index == r)
	{
		permute(data, 0, strlen(data) - 1, check);
		return;
	}

	// replace index with all possible elements. The condition
	// "end-i+1 >= r-index" makes sure that including one element
	// at index will make a combination with remaining elements
	// at remaining positions
	for (int i=start; i<=end && end-i+1 >= r-index; i++)
	{
		data[index] = arr[i];
		combinationUtil(arr, data, i+1, end, index+1, r, check);
	}
}
