#ifndef SPPEED
#define SPPEED

typedef struct mask {
  int length;
  unsigned int * dat;
  //the mask n bits, where n is the total number of columns, where 1 == still valid
  //number of 1s is the number of valid rows AND columns
} mask;

float fasterDeterminantOfAMatrix(matrix * in, mask * limit);
int numberOfBits(mask * in);

#endif