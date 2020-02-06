#include "library.h"
double distance(int x1, int y1, int x2, int y2){
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int main(){
  srand(123);
  char *filename = "picmaker.ppm";
  int x, y;
  int x_max = 1000;  /* width */
  int y_max = 1000;  /* height */
  unsigned char data[y_max][x_max * 3];
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
  write_image(x_max, y_max, data, filename);
  return 0;
}
