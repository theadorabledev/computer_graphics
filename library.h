#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef struct matrix{
  int width;
  int height;
  int **data;
} MATRIX;
MATRIX * generate_matrix(int width, int height);
void plot(MATRIX * matrix, int x, int y, int r, int g, int b);
int write_image(MATRIX * matrix, char * filename);
