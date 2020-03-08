#include "library.h"
void painful_circle(GRID * g, int cx, int cy, int color, int out_rad, int in_rad){
  for(double theta = 0; theta < 2 * M_PI; theta += .01){
    int x, y, xn, yn;
    ELEMENT * e = generate_element(20, color);
    get_point_polar(cx, cy, theta, out_rad, &x, &y);
    double n_theta = (M_PI + theta + (90 - rand() % 180) * M_PI / 180);
    for(int i = 0; i < 30; i++){
      get_point_polar(x, y, n_theta, 0.1 * (rand() % in_rad * 10), &xn, &yn);
      add_line(e, x, y, 0, xn, yn, rgb(0, 0, 0));
      x = xn;
      y = yn;
      n_theta = n_theta + ((22.5 - (rand() % 45)) * M_PI / 180);
    }
    plot_element(e, g);
  }
}

void let_there_be_light(){
  printf("Generating a piece titled \"Let There Be Light\"\n");
  int x_max = 1000;
  int y_max = 1000;
  GRID * g = generate_grid(x_max, y_max);
  painful_circle(g, 500, 500, rgb(255, 255, 255), 100, 100);
  write_image(g, "matrix_2.ppm");
  printf("Wrote image to file matrix_2.ppm\n");

}
int main(){
  srand(125);
  int x_max = 1000;
  int y_max = 1000;

  ELEMENT * e;
  int p = 0;
  for(double theta = 0; theta < 2 * M_PI; theta += (2 * M_PI / 100)){
    GRID * g = generate_grid(x_max, y_max);
    int x, y, x2, y2, x3, y3, x4, y4 = 0;

    get_point_polar(500, 500, theta, 500, &x, &y);
    get_point_polar(500, 500, theta, 50, &x2, &y2);
    get_point_polar(500, 500, M_PI + theta, 100, &x3, &y3);
    get_point_polar(500, 500, M_PI + theta, 200, &x4, &y4);
    for(int i = 0; i <= 40; i ++){
      for(int k = 0; k <= 1000; k += 1000){
	e = generate_element(8, rgb(255, 255, 255));
	int d = 6;
	int positions[] = {x, y, -400,
			   800, 800, -200,
			   200, 800, -100,
			   x2, y2, 0,
			   x3, y3, 400,
			   x2, y4, 1000,
			   25 * i, k, 100}; 
	bezier(e, positions, d, .01);
	int positions2[] = {x, y, -400,
			    800, 800, -200,
			    200, 800, -100,
			    x2, y2, 0,
			    x3, y3, 400,
			    x2, y4, 1000,
			    k, 25 * i, 100}; 
	bezier(e, positions2, d, .01);      
	plot_element(e, g);

      }
    }
    char image_name[20];
    snprintf(image_name, 20, "bezier_%03d.ppm", p);
    printf("%s\n", image_name);
    p++;
    write_image(g, image_name);
  }

  return 0;
}

