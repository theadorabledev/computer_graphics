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
void plot(MATRIX * matrix, int x, int y, int rgb){
  if (y < 0 || x < 0 || x >= matrix->width || y >= matrix->height)
    return;
  matrix->data[matrix->height - 1 - y][x] = rgb;
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
void draw_line_steep(MATRIX * matrix, int x1, int y1, int x2, int y2, int rgb){
  if(y2 < y1){
    int temp = x2;
    x2 = x1;
    x1 = temp;
    temp = y2;
    y2 = y1;
    y1 = temp;
  }
  int delta_y = y2 - y1;
  int delta_x = x2 - x1;
  int x_inc = delta_x/abs(delta_x);
  if(x_inc < 0)
    delta_x = -delta_x;
  int d = 2 * delta_x - delta_y;
  while(y1 <= y2){
    plot(matrix, x1, y1, rgb);
    if(d >= 0){
      x1 += x_inc;
      d -= (2 * delta_y);
    }
    y1 += 1;
    d += (2 * delta_x);
  }
}
void draw_line_gentle(MATRIX * matrix, int x1, int y1, int x2, int y2, int rgb){
  if(x2 < x1){
    int temp = x2;
    x2 = x1;
    x1 = temp;
    temp = y2;
    y2 = y1;
    y1 = temp;
  }
  int delta_y = y2 - y1;
  int delta_x = x2 - x1;
  int y_inc = delta_y/abs(delta_y);
  if(y_inc < 0)
    delta_y = -delta_y;
  int d = 2 * delta_y - delta_x;
  while(x1 <= x2){
    plot(matrix, x1, y1, rgb);
    if(d >= 0){
      y1 += y_inc;
      d -= (2 * delta_x);
    }
    x1 += 1;
    d += (2 * delta_y);
  }
}
void draw_line(MATRIX * matrix, int x1, int y1, int x2, int y2, int rgb){
  double slope = (y2 - y1) * 1.0 / (x2 - x1);
  if(abs(slope) <= 1){
    draw_line_gentle(matrix, x1, y1, x2, y2, rgb);
  }else{
    draw_line_steep(matrix, x1, y1, x2, y2, rgb);
  }
}
