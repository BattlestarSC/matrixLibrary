#ifndef SPPEED
#define SPPEED

typedef struct mask {
  int length;
  int numberOfSegments;
  unsigned int * dat;
} mask;
//the mask n bits, where n is the total number of columns, where 1 == still valid
//number of 1s is the number of valid rows AND columns

float fasterDeterminantOfAMatrix(matrix * in, mask * limit);
int numberOfBits(mask * in);
mask loadMask(int length, int newRestriction, mask * old);
int getValidBitLocation(mask * in, int whichOne);

#endif