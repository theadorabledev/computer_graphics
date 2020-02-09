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
  matrix->data[matrix->height - 1 - y][x] = rgb(r, g, b);
}
int rgb(int r, int g, int b){
     return((r&0x0ff)<<16)|((g&0x0ff)<<8)|(b&0x0ff);
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

void draw_line(MATRIX * matrix, int x1, int y1, int x2, int y2, int rgb){
  int temp = 0;
  if(x2 < x1){
    temp = x2;
    x2 = x1;
    x1 = temp;
  }
  if(y2 < y1){
    temp = y2;
    y2 = y1;
    y1 = temp;
  }
  int delta_y = y2 - y1;
  int delta_x = x2 - x1;
  double slope = delta_y * 1.0 / delta_x;
  printf("%f \n", slope);
  while(x1 < x2){
    plot(matrix, x1, y1, 0, 0, 0);
    printf("%d %d\n", x1, y1);
    int d = 2 * delta_y + delta_x;
    if(d > 0){
      y1 = y1 + 1;
      d = d + (2 * delta_x);
    }
    x1 = x1 + 1;
    d = d + (2 * delta_y);
  }
}
