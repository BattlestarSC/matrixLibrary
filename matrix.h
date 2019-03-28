#ifndef MAT_A
#define MAT_A

#define CLR_MEM 1

typedef struct vector {
  int length;
  float * data;
} vector;

typedef struct matrix {
  int noOfRows;
  int noOfColumns;
  vector * columns[];
} matrix;

typedef struct junk {
  matrix * dat;
  struct junk * next;
} junk;

matrix * loadMatrix(int rows, int columns, float * data);
void freeMatrix(matrix * victum);
matrix * addMatrix(matrix * orig, matrix * add);
matrix * multiplyMatrixByFactor(matrix * input, float factor);
matrix * dotMultiplyMatricies(matrix * a, matrix * b);
matrix * transposeMatrix(matrix * input);
matrix * addColumn(matrix * input, int dataLength, float * data);
float * flatMatrix(matrix * input, int extraSpaces);
matrix * appendRow(matrix * input, int dataLength, float * data);
float getElementByXY(int x, int y, matrix * in);
float * getRow(int row, matrix * in);
float * getColumn(int column, matrix * in);
int updateItemByXY(matrix * mat, int x, int y, float value);
int updateMatrixRow(matrix * mat, int row, vector * data);
vector * loadVector(int length, float * data);
vector * copyVector(vector * in);
int freeVector(vector * in);
int updateMatrixColumn(matrix * mat, int column, vector * data);
matrix * copyMatrix(matrix * input);
float negOneToThePower(int power); //speed/ease function
matrix * removeColumnWithJunk(matrix * in, int column, junk * head);
matrix * removeRowWithJunk(matrix * in, int row, junk * head);
matrix * matrixByFunction(float (*fp)(float), matrix * mat);
void addToJunkList(junk * head, matrix * dat);
float determinantOfMatrix(matrix * input, junk * head);
junk * initJunkList();
float detOf2By2(matrix * data);
void freeJunk(junk * head);
matrix * removeRow(matrix * in, int row);
matrix * columnRemovedMatrix(matrix * in, int column);



#endif