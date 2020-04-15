#include "library.h"


void line_test(){
  printf("Testing draw_line function.\n");

  char *filename = "line_test.ppm";
  int x, y;
  int x_max = 50;  /* width */
  int y_max = 50;  /* height */
  GRID * m = generate_grid(x_max, y_max);
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max; x++) {
      plot(m, x, y, 0, 0);
    }
  }
  // Quadrant 1
  draw_line(m, 25, 25, 0, 35, 25, 0, rgb(255, 0, 0));
  draw_line(m, 25, 25, 0, 35, 30, 0, rgb(255, 0, 0));
  draw_line(m, 25, 25, 0, 35, 35, 0, rgb(255, 0, 0));
  // Quadrant 2
  draw_line(m, 25, 25, 0, 30, 35, 0, rgb(0, 255, 0));
  draw_line(m, 25, 25, 0, 25, 35, 0, rgb(0, 255, 0));
  // Quadrant 3
  draw_line(m, 25, 25, 0, 15, 35, 0, rgb(0, 0, 255));
  draw_line(m, 25, 25, 0, 20, 35, 0, rgb(0, 0, 255));
  // QUADRANT 4
  draw_line(m, 25, 25, 0, 15, 30, 0, rgb(255, 255, 0));
  draw_line(m, 25, 25, 0, 15, 25, 0, rgb(255, 255, 0));
  // Quadrant 5
  draw_line(m, 25, 25, 0, 15, 20, 0, rgb(0, 255, 255));
  draw_line(m, 25, 25, 0, 15, 15, 0, rgb(0, 255, 255));
  // Quadrant 6
  draw_line(m, 25, 25, 0, 20, 15, 0, rgb(255, 0, 255));
  draw_line(m, 25, 25, 0, 25, 15, 0, rgb(255, 0, 255));
  // Quadrant 7
  draw_line(m, 25, 25, 0, 30, 15, 0, rgb(255, 255, 255));
  draw_line(m, 25, 25, 0, 35, 15, 0, rgb(255, 255, 255));
  // Quadrant 8
  draw_line(m, 25, 25, 0, 35, 20, 0, rgb(0, 255, 255));
  write_image(m, filename);
  printf("Test saved to line_test.ppm\n");
}
int main(){
  line_test();
  printf("Generating line image.\n");
  srand(125);
  int x, y;
  int x_max = 1000;  /* width */
  int y_max = 1000;  /* height */
  int cx = x_max / 2;
  int cy = y_max / 2;
  int xbu = x_max * 1.1;
  int xbl = - x_max * .1;
  int ybu = y_max * 1.1;
  int ybl = - y_max * .1;
  GRID * m = generate_grid(x_max, y_max);
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max; x++) {
      double d = distance(cx, cy, x, y);
      d = 1 - d / distance(cx, cy, x_max, y_max);
      plot(m, x, y, 0, rgb(255 * d, 255 * d,  255 * d));
    }
  }
  for(int i = 0; i < 1; i++){
    draw_line(m, rand() % xbu, rand() % ybu, 0, rand() % xbu + xbl, rand() % ybu + xbl, 0, rgb(rand() % 255, rand() % 255, rand() % 255));
  }
  for(int i = 0; i < 1000; i++){
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;
    int radius = rand() % 45 + 5;
    int x = rand() % x_max;
    int y = rand() % y_max;
    int spiral = rand() % 2;
    double offset = (2 * M_PI) / ((rand() % 50) + 1);
    if(!(rand() % 3)){
      for(double theta = 0; theta < M_PI * 2; theta += (2 * M_PI / pow(2, 15))){
	draw_line_polar(m, x, y, (spiral ? radius : radius / M_PI * theta), offset + theta, rgb(r * (theta / (M_PI * 2)), g * (theta / (M_PI * 2)), b * (theta / (M_PI * 2))));
      }
    }else{
      for(int k = 0; k < 5; k++){
	draw_line(m, rand() % xbu, rand() % ybu, 0, rand() % xbu + xbl, rand() % ybu + ybl, 0, rgb(r, g, b));
      }
    }

  }
  int r = 100;
  for(int x = cx - r; x <= cx + r; x++){
    for(int y = cy - r; y <= cy + r; y++){
      double d = distance(cx, cy, x, y);
      if(d <= r){
	d = 1 - (d / r);
	plot(m, x, y, 0, rgb(255 * d, 255 * d, 255 * d));
      }
    } 
  }
  for(double theta = 0; theta < M_PI * 2; theta += (2 * M_PI / pow(2, 7))){
    int x, y;
    get_point_polar(cx, cy, theta, 120, &x, &y);
    draw_line_polar(m, x, y, 1000, theta, rgb(255, 255, 255));
  }
  write_image(m, "line.ppm");
  printf("Wrote line image to line.ppm\n");
  return 0;
}
