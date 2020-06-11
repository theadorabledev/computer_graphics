#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <time.h>
#include "map/src/map.h"
#ifndef LIBRARY_H
#define LIBRARY_H
#define SWAP(a,b,type) {type ttttttttt=a;a=b;b=ttttttttt;}
#define PRINT_ARRAY(array, length){		\
    for(int flub = 0; flub < length; flub++)	\
      printf("%f\t", *(array + flub));		\
    printf("\n");}
#define STR_COPY(s) strcpy(calloc(strlen(s) + 1, 1), s)

typedef struct Matrix MATRIX;
typedef struct Matrix{
  int columns;
  int rows;
  double **data;
  int last_col;
  MATRIX * next;
} MATRIX;
typedef struct Grid{
  int width;
  int height;
  int **data;
  double **z_buffer;
} GRID;
typedef double* VECTOR;
typedef struct _Light LIGHT;
typedef struct _Light{
  VECTOR vector;
  VECTOR rgb;
} LIGHT;
typedef map_t(LIGHT*) LIGHT_MAP;
typedef struct Element ELEMENT;
typedef struct Element{
  MATRIX * edge_matrix;
  MATRIX * triangle_matrix;
  int color;
  int length;
  int triangle_length;
  ELEMENT * next_element;
  ELEMENT * children;
  VECTOR ambient_const;
  VECTOR diffuse_const;
  VECTOR specular_const;
  LIGHT_MAP lights;
  VECTOR viewpoint;
} ELEMENT;


GRID * generate_grid(int width, int height);
void free_grid(GRID * g);
void plot(GRID * grid, int x, int y, double z, int rgb);
void clear_grid(GRID * grid);
int write_image(GRID * grid, char * filename);
void draw_line(GRID * grid, int x1, int y1, double z1, int x2, int y2, double z2, int rgb);
void draw_line_polar(GRID * grid, int x1, int y1, double r, double theta, int rgb);
int rgb(int r, int g, int b);
void get_point_polar(int x, int y, double theta, double r, int *xn, int *yn);
double distance(int x1, int y1, int x2, int y2);
double degrees_to_radians(double degrees);
int radians_to_degrees(double radians);

#define M_PI 3.14159265358979323846

LIGHT * generate_light(double x, double y, double z, int r, int g, int b);
void add_light(ELEMENT * e, char * name, int x, int y, int z, int r, int g, int b);
void set_texture(ELEMENT * e, double ac, double dc, double sc);
void set_texture_rgb(ELEMENT * e, double ac_r, double ac_g, double ac_b,
		     double dc_r, double dc_g, double dc_b,
		     double sc_r, double sc_g, double sc_b);

ELEMENT * generate_element(int size, int color);
void free_element(ELEMENT * e);
void add_line(ELEMENT * e, int x, int y, int z, int x2, int y2, int z2);
void add_element(ELEMENT * list, ELEMENT * e);
void add_child(ELEMENT * parent, ELEMENT * child);
void plot_element(ELEMENT * e, GRID * g);

void set_color(ELEMENT * e, int color);

MATRIX * generate_matrix(int rows, int cols);
void free_matrix(MATRIX * m);
void print_matrix(MATRIX * m);
void ident(MATRIX * m);
void grow_matrix(MATRIX *m, int newcols);
void copy_matrix(MATRIX * a, MATRIX * b);
void multiply(MATRIX * a, MATRIX * b);


MATRIX * push_to_stack(MATRIX * m);
MATRIX * pop_from_stack(MATRIX * m);
void transform_stack(MATRIX * m, MATRIX * t);
void cone(ELEMENT * e, int x, int y, int z, double theta, double phi, double inner_angle, int radius, int res, int closed);
void speckle(ELEMENT * e, int x, int y, int z, int width, int height, int depth, int density , int radius, int spiked);
void flower(ELEMENT * e, int x, int y, int z, int theta, int phi, int variance,  int length, int tendrils, int bud);
void tendril(ELEMENT * e, int x, int y, int z, double theta, double phi, int variance, int length, int radius, int end, int res);
void scale(MATRIX * m, double x, double y, double z);
void project(MATRIX * m, double d);
void translate(MATRIX *m, double x, double y, double z);
void rotate_z_axis(MATRIX *m, double theta);
void rotate_y_axis(MATRIX *m, double theta);
void rotate_x_axis(MATRIX *m, double theta);
void rotate(MATRIX *m, double x_theta, double y_theta, double z_theta);

void circle(ELEMENT * e, double cx, double cy, double cz, double radius);
void bezier(ELEMENT * e, int positions[], int degree, double t_inc);
void hermite(ELEMENT * e, double data[], double t_inc);

void clear(ELEMENT * e);
void box(ELEMENT *e, double x, double y, double z, double width, double height, double depth);
void sphere(ELEMENT * e, double x, double y, double z, double r);
void torus(ELEMENT * e, double x, double y, double z, double r, double R);

char *str_replace(char *orig, char *rep, char *with);

#endif
