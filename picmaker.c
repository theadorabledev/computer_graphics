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
  int** matrix = generate_matrix(x_max, y_max);
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max * 3; x += 3) {
      plot(matrix, x / 3, y,
	   (x + y) & 255,
	   (x + 1 + y) & 255,
	   (x + 1 + y) & 255);
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
	  plot(matrix, x, y,
	       r + 5 * distance(x, y, c_x, c_y),
	       g + 5 * distance(x, y, c_x, c_y),
	       b + 5 * distance(x, y, c_x, c_y));
	}
      }
    }
  }
  
  write_image(matrix, x_max, y_max, filename);
  return 0;
}
