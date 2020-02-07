#include "library.h"
MATRIX * generate_matrix(int width, int height){
  MATRIX * m = malloc(sizeof(MATRIX));
  int* values = calloc(width * height, sizeof(int));
  int** rows = malloc(height * sizeof(int*));
  for (int i=0; i<height; ++i){
    rows[i] = values + i * width;
  }
  m->data = rows;
  m->width = width;
  m->height = height;
  return m;
}
void plot(MATRIX * matrix, int x, int y, int r, int g, int b){
  if (y < 0 || x < 0 || x >= matrix->width || y >= matrix->height)
    return;
  matrix->data[y][x] = ((r&0x0ff)<<16)|((g&0x0ff)<<8)|(b&0x0ff);
}
int write_image(MATRIX * matrix, char * filename){
  FILE * fp;
  fp = fopen(filename, "w");
  fprintf(fp, "P3\n %s\n %d\n %d\n %d\n", "# HELLO!", matrix->width, matrix->height, 255);
  for(int y = 0; y < matrix->height; y++){
    for(int x = 0; x < matrix->width; x++){
      fprintf(fp, "%d %d %d ", (matrix->data[y][x] >> 16) &0x0ff, (matrix->data[y][x] >> 8) &0x0ff, (matrix->data[y][x]) &0x0ff);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}
