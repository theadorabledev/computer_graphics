#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Matrix{
  int columns;
  int rows;
  double **data;
  int last_col;
} MATRIX;
typedef struct Grid{
  int width;
  int height;
  int **data;
} GRID;
typedef struct Element ELEMENT;
typedef struct Element{
  MATRIX * matrix;
  int color;
  ELEMENT * next_element;
  ELEMENT * children;
} ELEMENT;

GRID * generate_grid(int width, int height);
void plot(GRID * grid, int x, int y, int rgb);
int write_image(GRID * grid, char * filename);
void draw_line(GRID * grid, int x1, int y1, int x2, int y2, int rgb);
void draw_line_polar(GRID * grid, int x1, int y1, double r, double theta, int rgb);
int rgb(int r, int g, int b);
void get_point_polar(int x, int y, double theta, double r, int *xn, int *yn);
double distance(int x1, int y1, int x2, int y2);


#define M_PI 3.14159265358979323846


MATRIX * generate_matrix(int rows, int cols);
void free_matrix(MATRIX * m);
void print_matrix(MATRIX * m);
void ident(MATRIX * m);
void grow_matrix(MATRIX *m, int newcols);
void copy_matrix(MATRIX * a, MATRIX * b);
void multiply(MATRIX * a, MATRIX * b);
