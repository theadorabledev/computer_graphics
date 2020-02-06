#include "library.h"
void write_image(int width, int height, unsigned char matrix[height][width * 3], char * filename){

  FILE * fp;
  fp = fopen(filename, "w");
  fprintf(fp, "P3\n %s\n %d\n %d\n %d\n", "# HELLO!", width, height, 255);
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width * 3; x++){
      fprintf(fp, "%d ", matrix[y][x]);
      //printf("%d %d %d\n", x, y, matrix[y][x]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}
