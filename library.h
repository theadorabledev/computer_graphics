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
void plot(MATRIX * matrix, int x, int y, int rgb);
int write_image(MATRIX * matrix, char * filename);
void draw_line(MATRIX * matrix, int x1, int y1, int x2, int y2, int rgb);
void draw_line_polar(MATRIX * matrix, int x1, int y1, double r, double theta, int rgb);
int rgb(int r, int g, int b);

#define M_PI 3.14159265358979323846
