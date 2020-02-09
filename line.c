#include "library.h"
double distance(int x1, int y1, int x2, int y2){
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
void line_test(){
  printf("Testing draw_line function.\n");
  printf("Test saved to line_test.ppm\n");
  char *filename = "line_test.ppm";
  int x, y;
  int x_max = 50;  /* width */
  int y_max = 50;  /* height */
  MATRIX * m = generate_matrix(x_max, y_max);
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max; x++) {
      plot(m, x, y, 0);
    }
  }
  // Quadrant 1
  draw_line(m, 25, 25, 35, 25, rgb(255, 0, 0));
  draw_line(m, 25, 25, 35, 30, rgb(255, 0, 0));
  draw_line(m, 25, 25, 35, 35, rgb(255, 0, 0));
  // Quadrant 2
  draw_line(m, 25, 25, 30, 35, rgb(0, 255, 0));
  draw_line(m, 25, 25, 25, 35, rgb(0, 255, 0));
  // Quadrant 3
  draw_line(m, 25, 25, 15, 35, rgb(0, 0, 255));
  draw_line(m, 25, 25, 20, 35, rgb(0, 0, 255));
  // QUADRANT 4
  draw_line(m, 25, 25, 15, 30, rgb(255, 255, 0));
  draw_line(m, 25, 25, 15, 25, rgb(255, 255, 0));
  // Quadrant 5
  draw_line(m, 25, 25, 15, 20, rgb(0, 255, 255));
  draw_line(m, 25, 25, 15, 15, rgb(0, 255, 255));
  // Quadrant 6
  draw_line(m, 25, 25, 20, 15, rgb(255, 0, 255));
  draw_line(m, 25, 25, 25, 15, rgb(255, 0, 255));
  // Quadrant 7
  draw_line(m, 25, 25, 30, 15, rgb(255, 255, 255));
  draw_line(m, 25, 25, 35, 15, rgb(255, 255, 255));
  // Quadrant 8
  draw_line(m, 25, 25, 35, 20, rgb(0, 255, 255));
  write_image(m, filename);
}
int main(){
  srand(123);
  line_test();
    int x, y;
  int x_max = 50;  /* width */
  int y_max = 50;  /* height */
  MATRIX * m = generate_matrix(x_max, y_max);
  for (y = 0; y < y_max; y++) {
    for (x = 0; x < x_max; x++) {
      plot(m, x, y, rgb(255, 255, 255));
    }
  }
  write_image(m, "line.ppm");
  return 0;
}
