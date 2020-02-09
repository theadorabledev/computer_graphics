#include "library.h"
double distance(int x1, int y1, int x2, int y2){
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int main(){
  srand(123);
  char *filename = "line.ppm";
  int x, y;
  int x_max = 1000;  /* width */
  int y_max = 1000;  /* height */
  MATRIX * m = generate_matrix(x_max, y_max);
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max; x++) {
      plot(m, x, y, 255, 255, 255);
    }
  }
  
  draw_line(m, 0, 0, 10, 10, 0);
  //draw_line(m, 900, 900, 0, 0, 0);
  /*
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
	  plot(m, x, y,
	       r + 5 * distance(x, y, c_x, c_y),
	       g + 5 * distance(x, y, c_x, c_y),
	       b + 5 * distance(x, y, c_x, c_y));
	}
      }
    }
  }
  */
  write_image(m, filename);
  return 0;
}
