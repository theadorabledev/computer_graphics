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
void test_matrices(){
  printf("Testing Matrix Function!\n");
  MATRIX * a = generate_matrix(4, 4);
  ident(a);
  MATRIX * b = generate_matrix(4, 5);
  for(int r = 0; r < b->rows; r++)
    for(int c = 0; c < b->columns; c++)
      b->data[r][c] = r * 10 + c;
  for(int c = 0; c < b->columns; c++)
    b->data[3][c] = 1;
  printf("Matrix A:\n");
  print_matrix(a);
  printf("Matrix B:\n");
  print_matrix(b);
  printf("A * B:\n");
  multiply(a, b);
  print_matrix(b);
  printf("Converting A to scaling matrix\n");
  for(int r = 0; r < a->rows; r++)
    for(int c = 0; c < a->columns; c++)
      if(r == c)
	a->data[r][c] = 2;
  print_matrix(a);
  printf("A * B:\n");
  multiply(a, b);
  print_matrix(b);
  printf("Converting A to translation matrix\n");
  ident(a);
  a->data[0][3] = -10;
  a->data[1][3] = -10;
  a->data[2][3] = -10;
  print_matrix(a);
  printf("A * B:\n");
  multiply(a, b);
  print_matrix(b);
}
void four_angels_from_the_madness(){
  printf("Generating an image titled \"4 Angels From The Darkness\n");
  int x_max = 1000;
  int y_max = 1000;
  GRID * g = generate_grid(x_max, y_max);
  for(int x = 0; x < x_max; x++)
    for(int y = 0; y < y_max; y++)
      g->data[y][x] = rgb(255, 255, 255);

  painful_circle(g, 500, 500, 0, distance(0, 0, 500, 500), 200);
  painful_circle(g, 500, 500, 0, distance(0, 0, 500, 500) / 2, 200);

  for(int k = 0; k < 100; k++){
    ELEMENT * e = generate_element(20, rgb(255, 0, 0));
    int x = x_max / 2;
    int y = y_max / 2;
    int nx, ny = 0;
    double theta = rand() % 360 * (M_PI / 180);
    for(int i = 0; i < 20; i++){
      get_point_polar(x, y, theta, 0.1 * (rand() % 1000), &nx, &ny);
      add_line(e, x, y, 0, nx, ny, 0);
      x = nx;
      y = ny;
      theta = theta + ((90 - (rand() % 180)) * M_PI / 180);
    }
    plot_element(e, g);
    for(int i = 0; i < 10; i++){
      double orig_x = - e->matrix->data[0][0];
      double orig_y = - e->matrix->data[1][0];
      double orig_z = - e->matrix->data[2][0];
      translate(e->matrix, orig_x, orig_y, orig_z);
      e->color = rgb(rand() % 255, rand() % 255, rand() % 255);
      double factor = pow(rand() % 5 + 1, rand() % 2 ? 1 : -1);
      scale(e->matrix, factor, factor, 1);
      rotate_z_axis(e->matrix, rand() % 360 * (M_PI / 180));
      translate(e->matrix, - orig_x, - orig_y, - orig_z);
      translate(e->matrix, 200 - random() % 400, 200 - random() % 400, 1);
      plot_element(e, g);
    }
  }
  painful_circle(g, 500, 500, 0, 500, 15);
  for(int i = 0; i < 2; i++){
    for(int k = 0; k < 2; k++)
      painful_circle(g, k ? 250 : 750, i ? 250 : 750, rgb(255, 255, 255), 100, 20);
  }
  write_image(g, "matrix_1.ppm");
  printf("Wrote image to file matrix_1.ppm\n");
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
  test_matrices();
  srand(125);
  four_angels_from_the_madness();
  let_there_be_light();
  return 0;
}

