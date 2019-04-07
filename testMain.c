#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

#include "matrix.h"
#include "optimization.h"
#include "display.h"

float sigmoid(float x);

int main(void){
  /*
  float dataA[] = {1.1, 2.2, 3.3, 4.1, 5.2, 6.7, 7.0, 8.4, 9.9};
  //printf("DATA A Val 3 is %d should be 3.3\n", dataA[2]); //WHAT THE EVER LIVING FUCK!!!? PRINTS SOME DAMN MASSIVE NUMBER HERE BUT PRINTS CORRECTLY IN MATRIX FORMAT
  */
  float dataB[] = {0.1, 55.4, 605.3, 15.2, 12.5, 7.8, 800.9, 102.4, 111.111};
  //float dataC[] = {1.4, 5.2, 9.9};
  //matrix * a = loadMatrix(3, 3, dataA);
  //printf("Ran AFTER LOADING MATRIX a\n");
  float dataU[] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0};
  float dataI[] = {5.0,5.0,3.0,5.0,4.0,5.0,6.0,6.0,6.0};
  matrix * b = loadMatrix(3, 3, dataU);
  matrix * i = loadMatrix(3, 3, dataI);
  printf("Faster b is %f, should be 0\n", fasterDeterminantOfAMatrix(b, NULL));
  printf("Matrix should equal -12, is %f\n", fasterDeterminantOfAMatrix(i, NULL));
  //matrix * c = loadMatrix(1, 3, dataC);
  
  //matrix * d = copyMatrix(a);

  //skip testing
  
 /*  mask a = loadMask(16, 9, NULL);
  printf("a value is %d and is make of %d of 1 parts\n", *(a.dat), a.numberOfSegments);
  mask b = loadMask(16, 3, &a);
  printf("mask b is %d and of size %d\n", *(b.dat), b.length);
  printf("bit counting test, should get 15 and 14, %d %d\n", numberOfBits(&a), numberOfBits(&b));
  mask c = loadMask(16, 0, &b);
  mask d = loadMask(16, 1, &c);
  printf("test for getValidBitLocation, 1 should == 2, and 2 should == 4, actual: %d, %d\n", getValidBitLocation(&d, 1), getValidBitLocation(&d, 2));
  goto end; */
  /*
  float (*sig)(float) = sigmoid; 
  printf("D origional\n");
  printMatrix(d);
  printf("D by sigmoid\n");
  matrix * e = matrixByFunction(sig,d);
  printMatrix(e);
  printMatrix(a);
  printMatrix(d);
  float dataD[] = {3.3, 2.2, 5.5};
  vector * datd = loadVector(3, dataD);
  updateMatrixRow(d, 1, datd);
  printMatrix(d);
  printVector(datd);
  printVector(copyVector(datd));
  printf("%p\n", datd);
  printf("%d\n",updateMatrixColumn(d, 0, datd));
  printMatrix(d);
  matrix * e = columnRemovedMatrix(d, 0);
  printMatrix(e);
  matrix * f = rowRemovedMatrix(d, 1);
  printMatrix(f); */
  //Below is a list of 100 randomly generated decmial numbers
  //Gotta love free online tools
  
  float dataQ[] = {55.958,92.865,17.723,33.027,62.595,49.683,30.432,45.987,89.106,86.879,60.515,65.827,40.301,52.755,24.982,27.967,21.902,71.672,50.405,1.221,66.190,96.495,53.216,73.074,84.995,80.491,50.197,76.827,7.024,15.995,77.221,18.698,92.020,35.385,97.277,74.591,9.224,13.479,36.471,93.072,18.228,53.431,37.638,33.138,54.759,86.447,18.776,11.465,74.763,74.750,23.736,28.318,70.006,87.576,60.275,96.646,20.091,82.936,24.282,79.809,71.027,58.965,31.250,41.787,58.708,14.240,47.554,76.176,19.470,13.064,16.035,81.619,61.287,38.828,95.617,92.180,73.166,65.748,33.776,43.068,26.655,99.546,61.059,30.420,1.536,76.281,2.185,72.207,24.261,43.466,97.644,97.603,90.470,81.608,78.831,62.674,45.011,86.460,30.532,45.977};
  //matrix * s = loadMatrix(2, 2, dataA);
  //printMatrix(s);
  //printf("\nDet of s is %f\n", detOf2By2(s));
  //matrix * h = loadMatrix(2, 2, dataQ);
  //printMatrix(h);
  //matrix * sdf = loadMatrix(3,3, dataQ);
  //printMatrix(sdf);
  //printf("\nDeterminant of above is %f\n", determinantOfMatrix(sdf, NULL));
  //printf("\nDeterminant of H is %f\n", determinantOfMatrix(h,NULL));
  //matrix * l = loadMatrix(4, 4, dataQ);
  //printMatrix(l);
  //printf("The determinant of l is %f\n", determinantOfMatrix(l,NULL));
  //printf("The determinat of above is %f\n", determinantOfMatrix(h,NULL));
  matrix * q = loadMatrix(10, 10, dataQ);
  printMatrix(q);
  printf("The fast determinant of q is %f\n", fasterDeterminantOfAMatrix(q,NULL)); 
  //printf("The slow determinant of q is %f\n", determinantOfMatrix(q,NULL));

/* 
  matrix * randMatrix;
  for(int i=25;i<200;i++)
  {
    randMatrix = randomMatrix(i, i, 1000);
    printf("The determinant of a random %d by %d is %f\n", i, i, fasterDeterminantOfAMatrix(randMatrix, NULL));
    freeMatrix(randMatrix);
  }
 */

  float dataY[25] = {2.5, 17,  2,  4, 13, 72, 58, 22, 19, 12, 21, 14, 81, 84,  1, 76, 51, 74, 83, 63,  7, 99, 90, 85, 66};
  matrix * y = loadMatrix(5,5, dataY);
  printMatrix(y);
  printf("det of y is %f\n", fasterDeterminantOfAMatrix(y, NULL));
  y->columns[4]->data[3] = -3.5;
  printf("dat of y changed is %f\n", fasterDeterminantOfAMatrix(y, NULL));


  float newRow[5] = {12.0, 5.0, 8.0, 7.0, 2};
  matrix * v = appendRow(y, 5, newRow);
  float newColumn[] = {19, 22, 13, 9, 6, 14};
  matrix * p  = addColumn(v, 6, newColumn);
  printf("determinant of p is %f\n", fasterDeterminantOfAMatrix(p, NULL));
  
  float newTest[9] = { 10, 15, 3, 6, 1, 5, 2, 9, 17};
  matrix * u = loadMatrix(3, 3, newTest);
  printMatrix(u);
  printf("New det is %f\n", fasterDeterminantOfAMatrix(u, NULL));
  return 0;
}

float sigmoid(float x)
{
  return 1/exp(-x);
}