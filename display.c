#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "matrix.h"
#include "display.h"

/*
    Print a matrix to display
    If recieved a NULL, print 'NULL' as failure
*/

void printMatrix(matrix * input)
{   
    if(input == NULL){
      printf("\n\nNULL\n\n");
    } else {
      for(int i=0;i<input->noOfRows;i++)
      {
        printf("\n");
        for(int j=0;j<input->noOfColumns;j++)
       {
          printf("  %.4f  ", input->columns[j]->data[i]);
       }
     }
     printf("\n");
  }
}

/*
    print a flattened array to STDIN
*/
void printArray(int length, float * arr)
{
    printf("\n");
    for(int i=0;i<length;i++)
    {
        printf("  %.3f  ",arr[i]);
    }
    printf("\n");
}

/*
    print a vector to STDIN
*/
void printVector(vector * in)
{
    printArray(in->length, in->data);
}

/*
    print element by location (x,y)
*/
void printMatrixElementByXY(int x, int y, matrix * in)
{
    printf("\n%.5f\n",getElementByXY(x, y, in));
}