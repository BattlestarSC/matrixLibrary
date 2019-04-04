#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "optimization.h"


/* 

  The plan is to use a mask for columns no longer allowed
  DEPRECATED, LEFT FOR NOTES OF LOGIC

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
        if( ( ( *(limit->dat) ) & 0x1 << l) > 0)
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
        *(newLimit.dat) = *(limit->dat) ^ (0x1 << stillValid[i]);
        result = result + (negOneToThePower(i) * in->columns[stillValid[i]]->data[(newLimit.length - numberOfRemainingColumns)] * fasterDeterminantOfAMatrix(in, &newLimit));
      }
    }

  }
  }
  return result;
}
 */


/*
  Attempt two:
    Lets use some helper functions
*/
/*
    Supportive funtion, counts bits
*/
int numberOfBits(mask * in)
{
  int result = 0;
  for(int i=0;i<in->numberOfSegments;i++)
  {
    if(i != in->numberOfSegments-1)
    {
      for(int q=0;q<32;q++)
      {
        if((in->dat[i] & (0x1 << q) ) > 0) 
        {
          result++;
        }
      }
    }
    else
    {
      for(int x=0;x<(in->length - (32 * i));x++)
      {
        if((in->dat[i] & (0x1 << x) ) > 0) 
        {
          result++;
        }
      }
    }
    
  }
  return result;
}
//Load limits into a new mask
//accurate, but reversed, as in bit 3 is nnnnYnn, but that should be fine
//remember 0 indexing
mask loadMask(int length, int newRestriction, mask * old)
{
  mask result;
  result.length = length;
  if(length % 32 == 0)
  {
    if(length / 32 == 0 || length / 32 == 1)
    {
      result.numberOfSegments = 1;
      result.dat = malloc(sizeof(int));
    }
    else
    {
      result.numberOfSegments = length / 32;
      result.dat = malloc(sizeof(int) * result.numberOfSegments);
    }
  }
  else
  {
    if(length / 32 != 0)
    {
      result.numberOfSegments = (length / 32) + 1;
      result.dat = malloc(sizeof(int) * result.numberOfSegments);
    }
    else
    {
      result.numberOfSegments = 1;
      result.dat = malloc(sizeof(int) * result.numberOfSegments);
    }
  }
  
  if(!old)
  {
    for(int i=0;i<result.numberOfSegments;i++)
    {
      result.dat[i] = result.dat[i] & 0x0;
      result.dat[i] = ~ result.dat[i];
      if(newRestriction / 32 == i)
      {
        result.dat[i] = result.dat[i] ^ (0x1 << (newRestriction % 32));
      }
    }
  }
  else
  {
    for(int i=0;i<result.numberOfSegments;i++)
    {
      if(newRestriction / 32 == i)
      {
        result.dat[i] = old->dat[i] ^ (0x1 << (newRestriction % 32));
      }
      else
      {
        result.dat[i] = old->dat[i];
      }
    }
  }
  return result; 
}

//tests if bit n is valid
unsigned int bitValid(mask * in, int bit)
{
  unsigned int result = 0;
  if(!in)
  {
    result = 2;
    goto validEnd;
  }
  if(bit > in->length)
  {
    result = 3;
    goto validEnd;
  }
  if(in->numberOfSegments>1)
  {
    if((in->dat[bit/32] & (0x1 << (bit%32))) > 0)
    {
      result = 1;
    }
    goto validEnd;
  }
  if((in->dat[0] & (0x1 << (bit%32))) > 0)
  {
    result = 1;
  }

  validEnd:
    return result;
}

int getValidBitLocation(mask * limit, int number)
{
  int i = 0;
  int current = -1;
  while(i < number)
  {
    for(int x = 0;x<limit->length;x++)
    {
      if(bitValid(limit, x) == 1)
      {
        current = x;
        i++;
      }
    }
  }
  return current;
}

//Revised attempt two
float fasterDeterminantOfAMatrix(matrix * in, mask * limit)
{
  float result=0.0;

  //if invalid
  if(in->noOfColumns != in->noOfRows || in->noOfRows < 2)
  {
    goto fasterDeterminantOfAMatrixEnd; //Is this common/acceptable/good/not sloppy to have here? 
  }

  //base case, without mask
  if(in->noOfColumns == 2)
  {
    float ad = in->columns[0]->data[0] * in->columns[1]->data[1];
    float bc = in->columns[0]->data[1] * in->columns[1]->data[0];
    result = ad - bc;
    goto fasterDeterminantOfAMatrixEnd;
  }

  //base case, with mask
  if(limit){
    if(numberOfBits(limit) == 2)
    {
      float a = in->columns[getValidBitLocation(limit, 1)]->data[in->noOfRows - 2] * in->columns[getValidBitLocation(limit, 2)]->data[in->noOfRows - 1];
      float b = in->columns[getValidBitLocation(limit, 1)]->data[in->noOfRows - 1] * in->columns[getValidBitLocation(limit, 2)]->data[in->noOfRows - 2];
      result = a - b;
      goto fasterDeterminantOfAMatrixEnd;
    }
  }

  //if failure occured
  if(result != 0.0)
  {
    goto fasterDeterminantOfAMatrixEnd;
  }
  //Now break it down recursivally
  //TODO
  if(limit){
    for(int n = (numberOfBits(limit) - 1); n >= 0; n-- )
    {
      int nl = getValidBitLocation(limit, n + 1);
      mask newLimitation = loadMask(limit->length, nl, limit);
      result = result + (negOneToThePower(n) * in->columns[nl]->data[( in->noOfColumns - numberOfBits(limit) )] * fasterDeterminantOfAMatrix(in, &newLimitation));
    }
    //goto fasterDeterminantOfAMatrixEnd;
  }
  else
  {
    for(int t = (in->noOfColumns - 1); t >= 0; t--)
    {
      mask newLimit = loadMask(in->noOfColumns, t, NULL);
      result = result + (negOneToThePower(t) * in->columns[t]->data[0] * fasterDeterminantOfAMatrix(in, &newLimit));
    }
    //goto fasterDeterminantOfAMatrixEnd;
  }
  



  fasterDeterminantOfAMatrixEnd:
    return result;
} 