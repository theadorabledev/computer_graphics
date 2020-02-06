#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
int** generate_matrix(int width, int height);
void plot(int** matrix, int x, int y, int r, int g, int b);
int write_image(int** matrix, int width, int height, char * filename);
