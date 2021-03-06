#ifndef SPPEED
#define SPPEED

typedef struct mask {
  int length;
  int numberOfSegments;
  unsigned int * dat;
} mask;
//the mask n bits, where n is the total number of columns, where 1 == still valid
//number of 1s is the number of valid rows AND columns

float fasterDeterminantOfAMatrix(matrix * in, mask * limit, FILE * debugFile);
int numberOfBits(mask * in);
mask loadMask(int length, int newRestriction, mask * old);
unsigned int bitValid(mask * in, int bit);
int getValidBitLocation(mask * limit, int number);
float detOfMatrix(matrix * in);

#endif