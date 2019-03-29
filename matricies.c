#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"

/*
  This will allocate memory properly, save data in COLUMAR style for speed when in use
  I dont remember where I read the FORTRAN columar style was faster, but why the hell not?
*/
matrix * loadMatrix(int rows, int columns, float * data)
{
  matrix * result = malloc(  sizeof(matrix)  + (  sizeof(vector *) * columns ));  //allocate MATRIX + Num of Vectors
  if(CLR_MEM == 1){
    memset(result, 0, (  sizeof(matrix)  + (  sizeof(vector *) * columns ))); //CLEAR THE DAMN RAM
  }
  for(int i = 0; i < columns; i++) // for each column, allocate the RAM for the data section of the struct
  {
    result -> columns[i] = malloc(sizeof(vector) + sizeof(float) * rows); //so it failed before the pointer printing so maybe these just weren't defined ...?
    //printf("loading column %d\n", i); //DEBUG
    result -> columns[i] -> length = rows;
    result -> columns[i] -> data = malloc( sizeof(float) * rows);
    //printf("Matrix at %p\nColumn vector at %p\nVector data at %p\n", result, result->columns[i], result->columns[i]->data); //DEBUG
    if(CLR_MEM == 1)
    {
      memset(result -> columns[i] -> data, 0, ( sizeof(float) * rows)); //CLEAR THE DAMN RAM
    }
    for(int j = 0; j < rows; j++) // then load it from the input
    {
      result -> columns[i] -> data[j] = data[(columns * j) + i]; // every column item is accessed via (columns * j)=row(due to column number == row length) + i which is current column (as offset)
    }
  }
  result -> noOfRows = rows;
  result -> noOfColumns = columns;
  return result;
}

/*
  This will deallocate all the damn ram for a matrix properly
*/
void freeMatrix(matrix * victum)
{
  for(int i=0;i<victum->noOfColumns;i++){
    free(victum -> columns[i] -> data);
    free(victum -> columns[i]);
  }
  free(victum);
}

/* 
Add matricies, element wise, if of different sizes (1x3, 3x3), add by column
*/
matrix * addMatrix(matrix * orig, matrix * add)
{
  /*
    define resultant table
    if tables are of the same size, the result is of size of matrix orig
    if tables are of different, compatable size (either number of columns or rows are the same), the result is of size of the orig matrix, and it is added by row or column
    else failure; return a NULL address
  */
  matrix * result;
  if(orig->noOfColumns == add->noOfColumns && orig->noOfRows == add->noOfRows)
  {
    result = malloc(sizeof(matrix) + (sizeof(vector) * orig->noOfColumns));
    for(int i=0;i<orig->noOfColumns;i++){
      result->columns[i] = malloc(sizeof(vector));
      result->columns[i]->data = malloc(sizeof(float) * orig->noOfRows);
    }
    result->noOfColumns=orig->noOfColumns;
    result->noOfRows=orig->noOfRows;
  }
  else
  {
    result = NULL;
    return result;
  }

  //Determine proper maths method
  if(orig->noOfColumns==add->noOfColumns && orig->noOfRows == add->noOfRows) //Are they the exact same size? if so, lets do this properly
  //proper. element wise, for same size
  {
    for(int i=0;i<orig->noOfColumns;i++)
    {
      for(int j=0;j<orig->noOfRows;j++)
      {
        result->columns[i]->data[j] = orig->columns[i]->data[j] + add->columns[i]->data[j];
      }
    }
  }
  else
  //otherwise, lets hackily do it
  {
    if(orig->noOfColumns==add->noOfColumns && add->noOfRows==1)
    //by column, preferred, for whatever reason
    //THIS WILL ONLY WORK WHERE THERE IS orig->noOfColumns number of columns AND add->noOfRows == 1
    {
      for(int i=0;i<orig->noOfColumns;i++)
      {
        for(int j=0;j<orig->noOfRows;j++)
        {
          result->columns[i]->data[j] = orig->columns[i]->data[j] + add->columns[i]->data[0];
        }
      } 
    }
    else
    //by row
    //THIS WILL ONLY WORK FOR 1 columns and orig->noOfRows rows
    {
      for(int i=0;i<orig->noOfColumns;i++)
      {
        for(int j=0;j<orig->noOfRows;j++)
        {
          result->columns[i]->data[j] = orig->columns[i]->data[j] + add->columns[0]->data[j];
        }
      }
    }
    
  }
  //If ANY error occured
  if(!result || result==NULL){
    return NULL;
  }
  //otherwise
  return result;
  
}

/*
  This will multiply the ENTIRE matrix by a single coeffient named factor
  Process: create an array of result values then load into a matrix with above function loadMatrix()
*/
matrix * multiplyMatrixByFactor(matrix * input, float factor)
{
  float * values = malloc( sizeof(float) * input->noOfColumns * input->noOfRows);
  if(CLR_MEM == 1){
    memset(values, 0, ( sizeof(float) * input->noOfColumns * input->noOfRows)); //CLEAR THE DAMN RAM
  }
  /*
    in matrix natural format (row1, row2, etc), do the multiplication and load it into the values array in that order for injection into matrix
  */
  for(int j=0;j<input->noOfRows;j++)
  {
    for(int i=0;i<input->noOfColumns;i++)
    {
      values[(j*input->noOfColumns)+i] = input->columns[i]->data[j] * factor; 
    }
  }
  matrix * result = loadMatrix(input->noOfRows, input->noOfColumns, values);
  free(values);
  return result;
}

/* 
  Now the bitchy part, dot matrix multiplication
  Rules: in Row by Column format, result size = rowsOf1 by columnsOf2
  Input must be of size where rowsOf2 MUST equal ColumsOf1

  Method: create result float array 
          for row in a
            for column in b
              for item in each
                result[a * columnsOf2 + b] += rowOfA[item] * columnOfB[item]
          loadMatrix(result)
          free array
*/
matrix * dotMultiplyMatricies(matrix * a, matrix * b)
//Side note: Why the everliving fuck does a->noOfColumns work BEFORE compute but NOT AFTER?
{
  //If the two are not valid to be multiplied, return a NULL as failure
  if(a->noOfColumns != b->noOfRows)
  {
    return NULL;
  }

  //debug
  // if(a->noOfRows == 0 || a->noOfColumns == 0 || b->noOfRows == 0 || b->noOfColumns == 0)
  // {
  //   printf("FAILURE WITH INPUTS\ninput A dimentions %d  %d\ninput B dimentions %d  %d\n",a->noOfColumns, a->noOfRows, b->noOfColumns, b->noOfRows);
  //   return NULL;
  // }

  //More debug/testing
  //Don't know why this is needed, but it is
  int x, y;
  x = a->noOfRows;
  y = b->noOfColumns;
  //maybe solve failure with unmatched matricies
  int z;
  if (a->noOfRows > b->noOfColumns || a->noOfRows == b->noOfColumns)
  {
    z = a->noOfRows;
  }
  else
  {
    z = b->noOfColumns;
  }

  float * result = malloc(sizeof(float) * (x * y)); //allocate RAM
  if(CLR_MEM == 1)
  {
    memset(result, 0, (sizeof(float) * (x * y)));//Clear the DAMN RAM
  }
  //now actually, finally, do the multiplicaion. See logic notes above
  // for(int rowOfA = 0; rowOfA < x; rowOfA++)
  // {
  //   for(int columnOfB = 0; columnOfB < y; columnOfB++)
  //   {
  //     for(int i=0;i<y;b++)
  //     {
  //       result[(rowOfA * y)+columnOfB] += a->columns[i]->data[rowOfA] * b->columns[columnOfB]->data[i];
  //     }
  //   }
  // }
  //That shit didn't work

  for(int o=0;o<(x*y);o++) //for each item in the resultant array
  {
    for(int u=0;u<z;u++) //now for each item in the respective row/column
    {
      /*
        Logic: result per location = addition of each multiplication pair, with variance laterally for a and vertiaclly for b
        On incriment of O, o/y will equal 0 till o = y or larger, so we go by column in proper form as o%y resets when o/y incriments
      */
      result[o] = result[o] + (a->columns[u]->data[((int)(o/y))] * b->columns[((int)(o%y))]->data[u]); //I really like this solution
    }
  }


  //debug
  //printf("  %d  by  %d  total %d\n", (a->noOfRows), b->noOfColumns, (a->noOfRows * b->noOfColumns));
  //for(int t=0;t<(a->noOfColumns*b->noOfRows);t++)
  //{
  //  printf("  %f  ", result[t]);
  //}
  //printf("\n");

  //Now load it into matrix format
  //printf("result inputs %d  %d\n", x, y);
  matrix * finalResult = loadMatrix(x, y, result);

  //now free the temp array
  free(result);

  return finalResult;
}

/*
  This will transpose a matrix
  i.e. 1x3 -> 3x1
  each column will be changed into a row, then appended to a float array 
  that float array will be added to a matrix
*/
matrix * transposeMatrix(matrix * input)
{
  //save state information
  int x = input -> noOfRows;
  int y = input -> noOfColumns;

  //make target array 
  float * reform = malloc(sizeof(float) * x * y);
  if(CLR_MEM == 1){
   memset(reform, 0, (sizeof(float) * x * y)); //CLEAR THE DAMN RAM
  }
  //transform loop, column into row
  for(int i=0;i<y;i++)
  {
    for(int j=0;j<x;j++)
    {
      reform[(i*x)+j] = input -> columns[i] -> data[j];
    }
  }

  //now create a result matrix
  matrix * result = loadMatrix(y, x, reform);

  //free the temporary reform array
  free(reform);

  //return the matrix
  return result;
}

/*
  This will load the matrix into a float * array, then append the next column from the passed list
  Will return new matrix if secueeded, NULL if failed
*/
matrix * addColumn(matrix * input, int dataLength, float * data)
{
  //If not of compatable length
  if(dataLength != input->noOfRows)
  {
    return NULL;
  }
  matrix * result;
  float * oldData = malloc(sizeof(float) * input->noOfColumns * input->noOfRows);
  for(int i=0;i<(input->noOfRows*input->noOfColumns);i++)
  {
    oldData[i] = input->columns[i%input->noOfColumns]->data[i/input->noOfColumns];
  }
  result = loadMatrix(input->noOfRows, input->noOfColumns, oldData);
  free(oldData);
  result->columns[result->noOfColumns] = malloc(sizeof(vector));
  result->columns[result->noOfColumns]->length = input->noOfRows;
  float * res = malloc(sizeof(float) * dataLength);
  for(int q=0;q<dataLength;q++)
  {
    res[q] = data[q];
  }
  result->columns[result->noOfColumns]->data = res;
  result->noOfColumns = result->noOfColumns+1;
  return result;
}

/*
  This will flatten the matrix, then return the flattened array
  extra spaces are an option in case the array needs to be edited
*/
float * flatMatrix(matrix * input, int extraSpaces)
{
  float * result = malloc(sizeof(float) * (input->noOfRows * input->noOfColumns) + extraSpaces);
  if(CLR_MEM == 1){
    memset(result, 0, (sizeof(float) * (input->noOfRows * input->noOfColumns) + extraSpaces));
  }
  for(int i=0;i<input->noOfRows*input->noOfColumns;i++)
  {
    result[i] = input->columns[i%input->noOfColumns]->data[i/input->noOfColumns];
  }
  return result;
}

/*
  This will append a row to a matrix
  It will get a flattened array, then load a new matrix, then move and copy each column
*/
matrix * appendRow(matrix * input, int dataLength, float * data)
{
  //if wrong data
  if(dataLength != input->noOfColumns)
  {
    return NULL;
  }
  float * flatArray = flatMatrix(input, dataLength);
  for(int i=0;i<dataLength;i++)
  {
    flatArray[i+ (input->noOfColumns * input->noOfRows)] = data[i]; 
  }
  matrix * result = loadMatrix(input->noOfRows + 1, input->noOfColumns, flatArray);
  free(flatArray);
  return result;
}

/*
  Will allow getting element by row,column reference
    Failure: return 0.0
*/
float getElementByXY(int x, int y, matrix * in)
{
  if(x >= in->noOfRows || y >= in->noOfColumns)
  {
    return 0.000;
  }
  return in->columns[y]->data[x];
}

/*
  Return a single row as an array, return NULL on failure
*/
float * getRow(int row, matrix * in)
{
  if(row >= in->noOfRows)
  {
    return NULL;
  }
  float * result = malloc(sizeof(float) * in->noOfColumns);
  if(CLR_MEM == 1)
  {
    memset(result, 0, ((sizeof(float) * in->noOfColumns))); //CLEAR THE DAMN RAM
  }
  for(int i=0;i<in->noOfColumns;i++)
  {
    result[i] = in->columns[i]->data[row];
  }
  return result;
}

/*
  Return a sinlge column
  return NULL on failure
*/
float * getColumn(int column, matrix * in)
{
  if(column >= in->noOfColumns)
  {
    return NULL;
  }
  float * result = malloc(sizeof(float) * in->noOfRows);
  if(CLR_MEM == 1)
  {
    memset(result, 0, (sizeof(float)*in->noOfRows));
  }
  for(int i=0;i<in->noOfRows;i++)
  {
    result[i] = in->columns[column]->data[i];
  }
  return result;
}

/*
  This will edit an element by XY cords, return 0 if secuess, -1 if failure
*/
int updateItemByXY(matrix * mat, int x, int y, float value)
{
  //If wrong location is specified
  if(mat->noOfRows <= x || mat->noOfColumns <= y)
  {
    return -1;
  }
  mat->columns[y]->data[x] = value;
  return 0;
}

/*
  Will edit a row in a matrix
*/
int updateMatrixRow(matrix * mat, int row, vector * data)
{
  if(mat->noOfRows <= row || mat->noOfColumns != data->length)
  {
    return -1;
  }
  for(int i=0;i<data->length;i++)
  {
    updateItemByXY(mat, row, i, data->data[i]);
  }
  return 0;
}

/*
  Will load a float array into a vector
*/
vector * loadVector(int length, float * data)
{
  vector * result = malloc(sizeof(vector));
  if(CLR_MEM == 1)
  {
    memset(result,0,sizeof(vector));
  }
  result->data = malloc(sizeof(float) * length);
  if(CLR_MEM == 1)
  {
    memset(result->data, 0, sizeof(float) * length);
  }
  result->length = length;
  for(int i=0;i<result->length;i++)
  {
    result->data[i] = data[i];
  }
  return result;
}

/*
  Will copy a vector to a new address
*/
vector * copyVector(vector * in)
{
  vector * result = loadVector(in->length, in->data);
  return result;
}

/*
  Will deallocate RAM from vector
*/
int freeVector(vector * in)
{
  if(in == NULL || in->length <= 0 || in->data == NULL)
  {
    return -1;
  }
  int len = in->length;
  if(CLR_MEM == 1)
  {
    memset(in->data, 0, sizeof(float)*len);
    memset(in, 0, sizeof(vector));
  }
  free(in->data);
  free(in);
  return 0;
}

/*
  Will update a column in a matrix by replacing a vector
*/
int updateMatrixColumn(matrix * mat, int column, vector * data)
{
  if(column >= mat->noOfColumns || data->length != mat->noOfRows)
  {
    return -1;
  }
  //save old address
  vector * old = mat->columns[column];
  //copy over vector
  mat->columns[column] = copyVector(data);
  //clear old vector
  freeVector(old);
  return 0;
}

/*
  copy a matrix into a new memory location
*/
matrix * copyMatrix(matrix * input)
{
  float * flat = flatMatrix(input, 0);
  matrix * result = loadMatrix(input->noOfRows, input->noOfColumns, flat);
  free(flat);
  return result;
}

/*
  Quick optimization on -1^x
*/
float negOneToThePower(int power)
{
  if(power == 0)
  {
    return 1.0;
  }
  else if(power%2==1)
  {
    return -1.0;
  }
  else
  {
    return 1.0;
  }
}

/*
  This will load a matrix with a column missing

  Method:
    Copy matrix and remove column
*/
matrix * removeColumnWithJunk(matrix * in, int column, junk * head)
{
  matrix * result = copyMatrix(in);
  result->noOfColumns = result->noOfColumns -1;
  vector * old = result->columns[column];
  for(int i=column;i<in->noOfColumns;i++)
  {
    result->columns[i]=result->columns[i+1];
  }
  freeVector(old);
  if(head != NULL)
  {
    addToJunkList(head, result);
  }
  return result;
}

/*
  Now remove a row from a matrix
  Method:
  Copy matrix and redefine each column
*/
matrix * removeRowWithJunk(matrix * in, int row, junk * head)
{
  matrix * result = copyMatrix(in);
  result->noOfRows = result->noOfRows -1;
  float * temp;
  for(int i=0;i<in->noOfColumns;i++)
  {//For each column
    temp = malloc(sizeof(float) * result->noOfRows);
    for(int j=0;j<result->noOfRows;j++)
    {//For each item in the column
      if(j<row)
      {
        temp[j] = result->columns[i]->data[j];
      }
      else
      {
        temp[j] = in->columns[i]->data[j+1];
      }
      
    }
    freeVector(result->columns[i]);
    result->columns[i] = loadVector(result->noOfRows, temp);
    free(temp);
  }
  if(head!=NULL)
  {
    addToJunkList(head, result);
  }
  return result;
}

/*
  Update the matrix by the result of every element by function
*/
matrix * matrixByFunction(float (*fp)(float), matrix * mat)
{
  if(!fp)
  {
    return NULL;
  }
  matrix * result = copyMatrix(mat);
  for(int x=0;x<mat->noOfColumns;x++)
  {
    for(int y=0;y<mat->noOfRows;y++)
    {
      if(updateItemByXY(result, y, x, (fp)(getElementByXY(y,x,mat)))!=0) //if failure
      {
        freeMatrix(result);
        return NULL;
      }
    }
  }
  return result;
}

/*
  This will take the determinant of a matrix recursivelly. 
  Use expansion by cofactors
  To take a two by two, its 0,0 * 1,1 - 0,1 * 1,0
  For everything larger, its row wise, a * det(smaller matrix of rows and columns that a doesnt touch)
    - b * det() + c ...
  Matrix MUST be square, 2x2 or larger.
  Failure returns -0

  CURRENT: large matricies FAIL to finish
  2x2s are correct
*/

/*
  Junk collection
*/
void addToJunkList(junk * head, matrix * dat)
{
  junk * loc = head;
  while(loc -> next != NULL)
  {
    loc = loc->next;
  }
  loc->next = malloc(sizeof(junk));
  loc=loc->next;
  loc->dat = dat;
  loc->next = NULL;
}

/*
  Initalize junk list
*/
junk * initJunkList()
{
  junk * result = malloc(sizeof(junk));
  result->next=NULL;
  result->dat=NULL;
  return result;
}

/*
  Clear the junk list and free ALL data
*/
void freeJunk(junk * head)
{
  junk * loc = head;
  while(loc->next!=NULL)
  {
    if(loc->dat!=NULL)
    {
      freeMatrix(loc->dat);
    }
  }
  if(loc->dat!=NULL)
  {
    freeMatrix(loc->dat);
  }
  free(head);
}

/*
  Will recursivally take the determinant of the matrix
  If of 2x2, return determinant
  Otherwise, recurse without extra columns
*/
float determinantOfMatrix(matrix * input, junk * head)
{
  //Incompatiable case
  if(input->noOfColumns != input->noOfRows || input->noOfRows < 2)
  {
    return -0;
  }
  //Base case
  // Return the determinant of a 2x2
  if(input->noOfRows == 2)
  {
    return detOf2By2(input);
  }
  //temp vars
  float result = 0.0;
  //If larger than 2x2, break it down by row
  /*
    Logic comments: So, using the expansion by cofactors method, expand row 0
    For each item in the row, use the determinant of everything with the column and row removed * -1^i (to alternate 1 or -1) * vlaue
  */
  if(head == NULL){
    junk * jnk = initJunkList();
    for(int i=0;i<input->noOfColumns;i++){
      result = result + (negOneToThePower(i) * input->columns[i]->data[0] * determinantOfMatrix(removeColumnWithJunk(removeRowWithJunk(input, 0, jnk),i, jnk),jnk));
    }
    //prepare cleanup, schedule cleanup
    void(*freedat)(junk*) = freeJunk;
    void(*freed) = freedat;
    atexit(freed);
  }
  else
  {
    for(int i=0;i<input->noOfColumns;i++){
      result = result + (negOneToThePower(i) * input->columns[i]->data[0] * determinantOfMatrix(removeColumnWithJunk(removeRowWithJunk(input, 0, head),i, head),head));
    }
  }
  //Clean up RAM
  /* For whatever reason, this causes failure
  for(int i=0;i<input->noOfColumns*2;i++)
  {
    if(junk[i]!=NULL)
    {
      freeMatrix(junk[i]);
    }
  }
  free(junk); */
  /*
    free data at exit
  */
  
  return result;
}

/*
  Take the determinant for a two by two, prove that it works 
*/
float detOf2By2(matrix * data)
{
  if(data->noOfColumns != 2 || data->noOfRows != 2)
  {
    return 0.0;
  }
  float partA = data->columns[0]->data[0] * data->columns[1]->data[1];
  float partB = data->columns[0]->data[1] * data->columns[1]->data[0];
  return partA - partB;
}


/*
  Now remove a row from a matrix
  Method:
  Copy matrix and redefine each column
  Lame version, without auto garbage tracking
*/
matrix * removeRow(matrix * in, int row)
{
  matrix * result = copyMatrix(in);
  result->noOfRows = result->noOfRows -1;
  float * temp;
  for(int i=0;i<in->noOfColumns;i++)
  {//For each column
    temp = malloc(sizeof(float) * result->noOfRows);
    for(int j=0;j<result->noOfRows;j++)
    {//For each item in the column
      if(j<row)
      {
        temp[j] = result->columns[i]->data[j];
      }
      else
      {
        temp[j] = in->columns[i]->data[j+1];
      }
      
    }
    freeVector(result->columns[i]);
    result->columns[i] = loadVector(result->noOfRows, temp);
    free(temp);
  }
  return result;
}

/*
  This will load a matrix with a column missing

  Method:
    Copy matrix and remove column

  Method without auto garbage collection
*/
matrix * columnRemovedMatrix(matrix * in, int column)
{
  matrix * result = copyMatrix(in);
  result->noOfColumns = result->noOfColumns -1;
  vector * old = result->columns[column];
  for(int i=column;i<in->noOfColumns;i++)
  {
    result->columns[i]=result->columns[i+1];
  }
  freeVector(old);
  return result;
}

/*
  Generate a random matrix
*/
matrix * randomMatrix(int rows, int columns, int max)
{
  float inputs[rows*columns];
  if(max == 0)
  {
    for(int i = 0; i < (rows*columns);i++)
    {
      inputs[i] = rand() + (1.0 / rand());
      while(inputs[i] == 0.0)
      {
        inputs[i] = rand() + (1.0 / rand());
      }
    }
  }
  else
  {
    for(int i = 0; i < (rows*columns); i ++)
    {
      inputs[i] = (rand() % max) + (1.0 / rand());
      while(inputs[i] == 0.0)
      {
        inputs[i] = (rand() % max) + (1.0 / rand());
      }
    }
  }
  matrix * result = loadMatrix(rows, columns, inputs);
  return result;
  
}
/*
TODO:
  create random matrix (of fixed sizes)
  WORKING, needs to be speadup and optimised: Determinant of a matrix, recursivally
*/