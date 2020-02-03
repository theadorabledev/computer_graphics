#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double distance(int x1, int y1, int x2, int y2){
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int main(){
  srand(123);
  const char *filename = "picmaker.ppm";
  int x, y;
  const int x_max = 1000;  /* width */
  const int y_max = 1000;  /* height */
  unsigned char data[y_max][x_max * 3];
  FILE * fp;
  const char *comment = "# pretty circles!";
 
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max * 3; x += 3) {
      data[y][x] = (x + y) & 255;
      data[y][x + 1] = (x + 1 + y) & 255;
      data[y][x + 2] = (x + 2 + y) & 255;
    }
  }
  for(int i = 0; i < 100; i++){
    int radius = (rand() % 25) + 5;
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;
    int c_x = rand() % x_max;
    int c_y = rand() % y_max;
    
    for (y = 0; y < y_max; y++) {
      for (x = 0; x < x_max; x++) {
	if(distance(x, y, c_x, c_y) < radius){
	  data[y][x * 3] = r + 5 * distance(x, y, c_x, c_y);
	  data[y][x * 3 + 1] = g + 5 * distance(x, y, c_x, c_y);
	  data[y][x * 3 + 2] = b + 5 * distance(x, y, c_x, c_y);
	}
      }
    }
  }

  fp = fopen(filename, "w");
  fprintf(fp, "P3\n %s\n %d\n %d\n %d\n", comment, x_max, y_max,
          255);
  for(int y = 0; y < y_max; y++){
    for(int x = 0; x < x_max * 3; x++){
      fprintf(fp, "%d ", data[y][x]);
    }
  }
  fclose(fp);
  return 0;
}
