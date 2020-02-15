#include "library.h"

int main(){
  srand(123);
  printf("Generating picmaker image.\n");
  char *filename = "picmaker.ppm";
  int x, y;
  int x_max = 1000;  /* width */
  int y_max = 1000;  /* height */
  GRID * m = generate_grid(x_max, y_max);
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max * 3; x += 3) {
      plot(m, x / 3, y,
	   rgb((x + y) & 255,
	       (x + 1 + y) & 255,
	       (x + 2 + y) & 255));
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
	  plot(m, x, y,
	       rgb(r + 5 * distance(x, y, c_x, c_y),
		   g + 5 * distance(x, y, c_x, c_y),
		   b + 5 * distance(x, y, c_x, c_y)));
	}
      }
    }
  }
  
  write_image(m, filename);
  printf("Wrote picmaker image to image.ppm.\n");
  return 0;
}
