#include "syscall.h"

int partition(int arr[], int start, int end)
{
 
    int pivot; 
    int count = 0;
    int i;
    int j;
    int pivotIndex;
    int tmp;
    pivot = arr[start];
    for (i = start + 1; i <= end; i++) {
        if (arr[i] <= pivot)
            count++;
    }
 
    // Giving pivot element its correct position
    pivotIndex = start + count;
    tmp = arr[pivotIndex]; 
    arr[pivotIndex] = arr[start];
    arr[start] = tmp;
 
    // Sorting left and right parts of the pivot element
    i = start;
    j = end;
 
    while (i < pivotIndex && j > pivotIndex) {
 
        while (arr[i] <= pivot) {
            i++;
        }
 
        while (arr[j] > pivot) {
            j--;
        }
 
        if (i < pivotIndex && j > pivotIndex) {
            tmp = arr[i]; 
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }
 
    return pivotIndex;
}
 
void quickSort(int arr[], int start, int end)
{
 	int p;
    // base case
    if (start >= end)
        return;
 
    // partitioning the array
    p = partition(arr, start, end);
 
    // Sorting the left part
    quickSort(arr, start, p - 1);
 
    // Sorting the right part
    quickSort(arr, p + 1, end);
}

int main()
{
	int id;
	char* buffer;
	int size = 0;
	int n;
	int i = 0;
	int array[100];
	PrintString("Input n: ");
	n = ReadInt();
	for (i; i < n; i++)
	{
		PrintString("Input elements: ");
		array[i] = ReadInt();
	}
	quickSort(array, 0, n - 1);

	CreateFile("quicksort.txt");
	id = Open("quicksort.txt", 0);
	for (i = 0; i < n; i++)
	{
		size = ToString(buffer, array[i]);
		Write(buffer, size - 1, id);
		Write("\r\n", 2, id);
	}
	Halt();
}