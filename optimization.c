#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "optimization.h"

/*
    Supportive funtion, counts bits
*/
int numberOfBits(mask * in)
{
  int result = 0;
  for(int i = 0; i < in->length ; i++)
  {
    //If the bit is a one
    if( ( (  *(in->dat) >> i  ) & 0x1 ) > 0)
    {
      result++;
    }
  }
  return result;
}

/*
  The plan is to use a mask for columns no longer allowed
*/
float fasterDeterminantOfAMatrix(matrix * in, mask * limit)
{
  float result = 0.0;

  //if invalid
  if(in->noOfRows != in->noOfColumns || in->noOfRows < 2)
  {
    result = -1.0;;
  }
  //if native 2x2
  if(in->noOfRows == 2 && result != -1.0)
  {
    result = detOf2By2(in);
  }

  if(result == 0.0){
  if(limit != NULL)
  {
    //if 2x2 by mask, base case
    if(numberOfBits(limit) == 2)
    {
      //Now, set the matrix by x,y (row, column)
      int y[2] = {-1, -1};
      //These will always be the lowest two items because a 4x4 will be made of 4 3x3s which is in turn made of 3 2x2s, all of which are bottom row
      int x[2] = {in->noOfRows-1, in->noOfRows-2};

      for(int i = 0; i < limit->length; i++)
      {
        if(  (  (  *(limit->dat) >> i ) & 0x1 ) > 0 )
        {
          if(y[0] == -1)
          {
            y[0] = i;
          }
          else
          {
            y[1] = i;
          }
        }
      }
      float a = in->columns[y[0]]->data[x[0]] * in->columns[y[1]]->data[x[1]];
      float b = in->columns[y[0]]->data[x[1]] * in->columns[y[1]]->data[x[0]];
      result = a - b;
    }

  }
  else
  //now recurse
  {

    if(limit == NULL)
    {

      for(int i = 0 ; i < in->noOfColumns ; i++)
      {
        mask limiter;
        limiter.length = in->noOfColumns;
        limiter.dat = malloc( (limiter.length/8) + 1);
        *(limiter.dat) = *(limiter.dat) & 0x0; //clear it
        *(limiter.dat) = ~*(limiter.dat); //now make them all valid

        *(limiter.dat) = *(limiter.dat) ^ (0x1 << i); //XOR the offending bit against a left shifted 1 by the number of places
        result = result + (negOneToThePower(i) * fasterDeterminantOfAMatrix(in, &limiter) * in->columns[i]->data[0]);
      }

    }
    else
    {
      int stillValid[numberOfBits(limit)];
      int numberOfRemainingColumns=0;
      for(int l = 0; l < limit->length; l++)
      {
        if( ( *(limit->dat) >> l ) > 0)
        {
          stillValid[numberOfRemainingColumns] = l;
          numberOfRemainingColumns++;
        }
      }

      //now reduce it from the mask
      for(int i = 0; i < numberOfRemainingColumns; i++)
      {
        mask newLimit;
        newLimit.length = limit->length;
        newLimit.dat = malloc((newLimit.length/8) + 1);
        *(newLimit.dat) = *(limit->dat);
        *(newLimit.dat) = (((*(newLimit.dat) >> stillValid[i]) ^ 0x1) << stillValid[i]);
        result = result + (negOneToThePower(i) * in->columns[stillValid[i]]->data[(newLimit.length - numberOfRemainingColumns)] * fasterDeterminantOfAMatrix(in, &newLimit));
      }
    }

  }
  }
  return result;
}



/*
  Attempt two:
    Lets use some helper functions
*/
mask * loadMask(int length, int *validColumns, int columnLength)
{
  mask * result = malloc(sizeof(mask));
  result->length = length;
  result->dat = malloc((length/8)+1);
  *(result->dat) = *(result->dat) & 0x0;
  *(result->dat) = ~*(result->dat);
  for(int i=0;i<columnLength;i++)
  {
    *(result->dat) = (*(result->dat) >> i) ^ 0x1;
  }
  return result;
}
