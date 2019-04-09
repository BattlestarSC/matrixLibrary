#ifndef DISPLAY
#define DISPLAY

void printMatrix(matrix * input);
void printArray(int length, float * arr);
void printVector(vector * in);
void printMatrixElementByXY(int x, int y, matrix * in);
void printMatrixToFile(matrix * input, FILE * fp);

#endif