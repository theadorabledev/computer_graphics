#include "library.h"
int** generate_matrix(int width, int height){
  int* values = calloc(width * height, sizeof(int));
  int** rows = malloc(height * sizeof(int*));
  for (int i=0; i<height; ++i){
    rows[i] = values + i * width;
  }
  return rows;
}
void plot(int** matrix, int x, int y, int r, int g, int b){
  matrix[y][x] = ((r&0x0ff)<<16)|((g&0x0ff)<<8)|(b&0x0ff);
}
int write_image(int** matrix, int width, int height, char * filename){
    FILE * fp;
  fp = fopen(filename, "w");
  fprintf(fp, "P3\n %s\n %d\n %d\n %d\n", "# HELLO!", width, height, 255);
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      fprintf(fp, "%d %d %d ", (matrix[y][x] >> 16) &0x0ff, (matrix[y][x] >> 8) &0x0ff, (matrix[y][x]) &0x0ff);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}
void write_image_2(int width, int height, unsigned char matrix[height][width * 3], char * filename){
  FILE * fp;
  fp = fopen(filename, "w");
  fprintf(fp, "P3\n %s\n %d\n %d\n %d\n", "# HELLO!", width, height, 255);
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width * 3; x++){
      fprintf(fp, "%d ", matrix[y][x]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

