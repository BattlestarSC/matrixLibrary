#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "optimization.h"
#include "display.h"

#define DEBUG 1
#define DEBUG_FILE "out\\debug.txt"

/*
  Attempt two:
    Lets use some helper functions
*/
/*
    Supportive funtion, counts bits
    //WILL FAIL AT > 32 DUE TO NOT PROPERLY READING LENGTH
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
float fasterDeterminantOfAMatrix(matrix * in, mask * limit, FILE * debugFile)
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
        float posaa = getElementByXY(0,0,in);
        float posab = getElementByXY(1,1,in);
        float posba = getElementByXY(1,0,in);
        float posbb = getElementByXY(0,1,in);
        float a = posaa * posab;
        float b = posba * posbb;
        result = a - b;
        if(DEBUG == 1)
        {
          fprintf(debugFile, "Matrix was determined to be 2x2, matrix is as follows\n");
          printMatrixToFile(in, debugFile);
          fprintf(debugFile, "\nCalculations: (%f * %f) - (%f * %f) = %f\n\n\n", posaa, posab, posba, posbb, result);
        }
      }

      //base case, WITH LIMIT
      //Limit will used ONLY with supportive functions to assure same formating
      if(limit)//check if limit exists
      {
        //base case
        if(numberOfBits(limit) == 2)
        {
          float aa = getElementByXY(in->noOfRows-2, getValidBitLocation(limit, 0), in);
          float ab = getElementByXY(in->noOfRows-1, getValidBitLocation(limit, 1), in);
          float ba = getElementByXY(in->noOfRows-1, getValidBitLocation(limit, 0), in);
          float bb = getElementByXY(in->noOfRows-2, getValidBitLocation(limit, 1), in);

          float q = aa * ab;
          float e = ba * bb;

          result = q - e;
          if(DEBUG == 1)
          {
            for(int n=0;n<(limit->length - numberOfBits(limit));n++)
            {
              fprintf(debugFile, "--");
            }
            fprintf(debugFile, "Limit mask has 2 valid spots, %d, %d\n", getValidBitLocation(limit, 0), getValidBitLocation(limit, 1));
            fprintf(debugFile, "Computations: %f = (%f * %f) - (%f * %f)\nReturning result\n", result, aa, ab, ba, bb);
          }
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
            if(DEBUG == 1)
            {
              for(int n=0;n<(limit->length - numberOfBits(limit));n++)
              {
                fprintf(debugFile, "--");
              }
              fprintf(debugFile, "new limit applied to position %d\nnew limit: \n", i);
              for(int q=0;q<limit->length;q++)
              {
                if(bitValid(&newLimit, q) == 1)
                {
                  fprintf(debugFile, "  %d", q);
                }
                fprintf(debugFile, "\n");
              }
              fprintf(debugFile, "Old limit\n");
              for(int q=0;q<limit->length;q++)
              {
                if(bitValid(limit, q) == 1)
                {
                  fprintf(debugFile, "  %d", q);
                }
                fprintf(debugFile, "\n");
              }
              fprintf(debugFile, "\nResult = itself plus %f * %f * recurse\n\n", negOneToThePower(i), getElementByXY(in->noOfRows - numberOfBits(limit), getValidBitLocation(limit, i), in));
            }
            result = result + (negOneToThePower(i) * getElementByXY(in->noOfRows - numberOfBits(limit), getValidBitLocation(limit, i), in) * fasterDeterminantOfAMatrix(in, &newLimit, debugFile));
          }
        }
        
      }
      else
      {//if not already computed, or too large
        if(in->noOfColumns > 2 && in->noOfColumns < 64)
        {
          if(DEBUG == 1)
          {
            fprintf(debugFile, "Taking determinant of the following matrix\n");
            printMatrixToFile(in, debugFile);
            fprintf(debugFile, "\n\n\n");
          }
          for(int i=0;i<in->noOfColumns;i++)
          {
            mask limiter = loadMask(in->noOfColumns, i, NULL);
            if(DEBUG == 1)
            {
              fprintf(debugFile, "\nNew limit is at position %d\nResult will equal %f * %f * recurse\n\n", i, negOneToThePower(i), getElementByXY(0, i, in));
            }
            result = result + (negOneToThePower(i) * getElementByXY(0, i, in) * fasterDeterminantOfAMatrix(in, &limiter, debugFile));
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
  else
  {
    printf("General run failure\n");
  }
  
  return result;
}

float detOfMatrix(matrix * in)
{
  float result = 0.0;
  if(DEBUG == 1)
  {
    FILE * debugFile = fopen(DEBUG_FILE, "r");
    if(debugFile)
    {
      fclose(debugFile);
      debugFile = fopen(DEBUG_FILE, "a");
      for(int i=0;i<80;i++)
      {
        fprintf(debugFile, "-");
      }
      fprintf(debugFile, "\n");
    }
    else
    {
      debugFile = fopen(DEBUG_FILE, "w+");
    }
    result = fasterDeterminantOfAMatrix(in, NULL, debugFile);
    fclose(debugFile);
  }
  else
  {
    result = fasterDeterminantOfAMatrix(in, NULL, NULL);
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