#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "optimization.h"

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
  if(number > numberOfBits(limit))
  {
    printf("invalid input to getValidBitLocation\n");
    return -2;
  }
  int current = -1;
  for(int i=0;i<limit->length;i++)
  {
    if(bitValid(limit, i) == 1)
    {
      current++;
    }
    if(current == number)
    {
      return i;
    }
  }
  printf("getValidLocation General failure (-1)\n");
  return -1;
}

//attempt three, use the surrportive functions and try again
float fasterDeterminantOfAMatrix(matrix * in, mask * limit)
{
  //debug
  //printf("%p of mask \n",limit);
  float result = 0.0;
  //if not invalid
  if(in){
    if(!(in->noOfRows != in->noOfColumns || in->noOfColumns < 2))
    {
      //base case, no need to itterate/recurse
      if(in->noOfColumns == 2)
      {
        float a = in->columns[0]->data[0] * in->columns[1]->data[1];
        float b = in->columns[0]->data[1] * in->columns[1]->data[0];
        result = a - b;
      }

      //base case, WITH LIMIT
      //Limit will used ONLY with supportive functions to assure same formating
      if(limit)//check if limit exists
      {
        //base case
        if(numberOfBits(limit) == 2)
        {
          float q = in->columns[getValidBitLocation(limit, 0)]->data[in->noOfRows-2] * in->columns[getValidBitLocation(limit, 1)]->data[in->noOfRows-1];
          float e = in->columns[getValidBitLocation(limit, 0)]->data[in->noOfRows-1] * in->columns[getValidBitLocation(limit, 1)]->data[in->noOfRows-2];
          result = q * e;
        }
        //now recurse and break it down
        else
        {
          for(int i=0;i<numberOfBits(limit);i++)
          {
            mask newLimit = loadMask(limit->length, getValidBitLocation(limit, i), limit);
            //debug 
            //printf("--%p < - > %p   old < - > new\n", limit, &newLimit);
            //POSSIBLE SUBTRACTION SEGFAULT, but check getValidBitLocation first
            result = result + (negOneToThePower(i) * in->columns[getValidBitLocation(limit, i)]->data[in->noOfRows - numberOfBits(limit)] * fasterDeterminantOfAMatrix(in, &newLimit));
          }
        }
        
      }
      else
      {//if not already computed, or too large
        if(in->noOfColumns > 2 && in->noOfColumns < 64)
        {
          for(int i=0;i<in->noOfColumns;i++)
          {
            mask limiter = loadMask(in->noOfColumns, i, NULL);
            result = result + (negOneToThePower(i) * in->columns[i]->data[0] * fasterDeterminantOfAMatrix(in, &limiter));
          }
        }
        else
        {//if not already done
          if(in->noOfColumns > 64)
          {
            //TODO: gaussian elimination method for fucking massive matricies
            int g = 0; //just avoid a gcc error, in a todo section
          }
        }
        
      }
      




    }
  }
  return result;
}










//Revised attempt two
//DEPRECATED, DEAD, given up on
/*
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
*/