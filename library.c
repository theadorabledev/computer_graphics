#include "library.h"
GRID * generate_grid(int width, int height){
  GRID * m = malloc(sizeof(GRID));
  int* values = calloc(width * height, sizeof(int));
  int** rows = malloc(height * sizeof(int*));
  for (int i=0; i<height; ++i){
    rows[i] = values + i * width;
  }
  m->data = rows;
  m->width = width;
  m->height = height;
  return m;
}
void plot(GRID * grid, int x, int y, int rgb){
  if (y < 0 || x < 0 || x >= grid->width || y >= grid->height)
    return;
  grid->data[grid->height - 1 - y][x] = rgb;
}
int rgb(int r, int g, int b){
  return((r&0x0ff)<<16)|((g&0x0ff)<<8)|(b&0x0ff);
}
double distance(int x1, int y1, int x2, int y2){
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
void get_point_polar(int x, int y, double theta, double r, int *xn, int *yn){
  *xn = x + (r * cos(theta));
  *yn = y + (r * sin(theta));
}
int write_image(GRID * grid, char * filename){
  FILE * fp;
  fp = fopen(filename, "w");
  fprintf(fp, "P3\n %s\n %d\n %d\n %d\n", "# HELLO!", grid->width, grid->height, 255);
  for(int y = 0; y < grid->height; y++){
    for(int x = 0; x < grid->width; x++){
      fprintf(fp, "%d %d %d ", (grid->data[y][x] >> 16) &0x0ff, (grid->data[y][x] >> 8) &0x0ff, (grid->data[y][x]) &0x0ff);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}
void clear_grid(GRID * grid){
  for(int y = 0; y < grid->height; y++)
    for(int x = 0; x < grid->width; x++)
      grid->data[y][x] = rgb(255, 255, 255);
}
void draw_line_steep(GRID * grid, int x1, int y1, int x2, int y2, int rgb){
  if(y2 < y1){
    int temp = x2;
    x2 = x1;
    x1 = temp;
    temp = y2;
    y2 = y1;
    y1 = temp;
  }
  int delta_y = y2 - y1;
  int delta_x = x2 - x1;
  int x_inc = delta_x/abs(delta_x);
  if(x_inc < 0)
    delta_x = -delta_x;
  int d = 2 * delta_x - delta_y;
  while(y1 <= y2){
    plot(grid, x1, y1, rgb);
    if(d >= 0){
      x1 += x_inc;
      d -= (2 * delta_y);
    }
    y1 += 1;
    d += (2 * delta_x);
  }
}
void draw_line_vertical(GRID * grid, int x1, int y1, int x2, int y2, int rgb){
  if(y2 < y1){
    int temp = y2;
    y2 = y1;
    y1 = temp;
  }
  while(y1 <= y2){
    plot(grid, x1, y1, rgb);
    y1 += 1;
  }  
}
void draw_line_gentle(GRID * grid, int x1, int y1, int x2, int y2, int rgb){
  if(x2 < x1){
    int temp = x2;
    x2 = x1;
    x1 = temp;
    temp = y2;
    y2 = y1;
    y1 = temp;
  }
  int delta_y = y2 - y1;
  int delta_x = x2 - x1;
  int y_inc = delta_y ? delta_y/abs(delta_y) : 1;
  if(y_inc < 0)
    delta_y = -delta_y;
  int d = 2 * delta_y - delta_x;
  while(x1 <= x2){
    plot(grid, x1, y1, rgb);
    if(d >= 0){
      y1 += y_inc;
      d -= (2 * delta_x);
    }
    x1 += 1;
    d += (2 * delta_y);
  }
}
void draw_line(GRID * grid, int x1, int y1, int x2, int y2, int rgb){
  //printf("%-4d %-4d - %-4d %-4d \n", x1, y1, x2, y2);
  double slope = (x2 - x1) ? ((y2 - y1) * 1.0 / (x2 - x1)) : INT_MAX;
  if(fabs(slope) <= 1.0){
    draw_line_gentle(grid, x1, y1, x2, y2, rgb);
  }else if(slope == INT_MAX){
    draw_line_vertical(grid, x1, y1, x2, y2, rgb);
  }else{
    draw_line_steep(grid, x1, y1, x2, y2, rgb);
  }
}
void draw_line_polar(GRID * grid, int x1, int y1, double r, double theta, int rgb){
  draw_line(grid, x1, y1, x1 + (int) (r * cos(theta)), y1 + (int) (r * sin(theta)), rgb);
}

ELEMENT * generate_element(int size, int color){
  ELEMENT *e = (ELEMENT *)malloc(sizeof(ELEMENT));
  e->matrix = generate_matrix(4, size * 2);
  e->color = color;
  e->length = 0;
  e->next_element = NULL;
  e->children = NULL;
  return e;
}
void add_col(ELEMENT  * e, int x, int y, int z){
  double ** m = e->matrix->data;
  m[0][e->length] = x;
  m[1][e->length] = y;
  m[2][e->length] = z;
  m[3][e->length] = 1;
  e->length += 1;
  if(e->length == e->matrix->columns)
    grow_matrix(e->matrix, (e->matrix->columns * 3)/2);
}
void copy_last_col(ELEMENT * e){
  double ** m = e->matrix->data;
  int l = e->length - 1;
  add_col(e, m[0][l], m[1][l], m[2][l]);
}
void add_line(ELEMENT * e, int x, int y, int z, int x2, int y2, int z2){
  add_col(e, x, y, z);
  add_col(e, x2, y2, z2);
}
void add_element(ELEMENT * list, ELEMENT * e){
  while(list->next_element)
    list = list->next_element;
  list->next_element = e;
}
void add_child(ELEMENT * parent, ELEMENT * child){
  if(parent->children)
    add_element(parent->children, child);
  else
    parent->children = child;
}
  
void plot_element(ELEMENT * e, GRID * g){
  if(!e)
    return;
  MATRIX *m = e->matrix;
  for(int c = 0; c < m->columns; c += 2){
    draw_line(g, (int)m->data[0][c], (int)m->data[1][c], (int)m->data[0][c + 1], (int)m->data[1][c + 1], e->color);
  }
  plot_element(e->next_element, g);
  plot_element(e->children, g);

}

MATRIX * generate_matrix(int rows, int cols){
  double **tmp;
  int i;
  MATRIX *m;
  tmp = (double **)calloc(rows * sizeof(double *), 1);
  for (i=0;i<rows;i++) {
    tmp[i]=(double *)calloc(cols * sizeof(double), 1);
    }
  m=(MATRIX *)calloc(sizeof(MATRIX), 1);
  m->data=tmp;
  m->rows = rows;
  m->columns = cols;
  m->last_col = 0;

  return m;
}
void free_matrix_data(MATRIX * m){
  for (int i = 0; i < m->rows; i++) {
    free(m->data[i]);
  }
  free(m->data);
}
void free_matrix(MATRIX * m){
  free_matrix_data(m);
  free(m);
}
void print_matrix(MATRIX * m){
  printf("%d x %d \n", m->rows, m->columns);
  for(int r = 0; r < m->rows; r++){
    for(int c = 0; c < m->columns; c++){
      printf("%-7.2f ", m->data[r][c]);
    }
    printf("\n");
  }
  printf("\n");
}
void grow_matrix(MATRIX *m, int newcols){
  newcols = newcols + (newcols % 2);
  for (int i = 0; i < m->rows; i++) {
    double * d = calloc(newcols, sizeof(double));
    memcpy(d, m->data[i], sizeof(double) * m->columns);
    free(m->data[i]);
    m->data[i] = d;
    //m->data[i] = realloc(m->data[i], newcols*sizeof(double));
  }
  
  m->columns = newcols;
}
void copy_matrix(MATRIX * a, MATRIX * b){
  for (int r=0; r < a->rows; r++) 
    for (int c=0; c < a->columns; c++)  
      b->data[r][c] = a->data[r][c];  
}
void multiply(MATRIX * a, MATRIX * b){
  MATRIX * t = generate_matrix(a->rows, b->columns);
  for (int r=0; r < a->rows; r++){
    for (int c=0; c < b->columns; c++){
      for(int i = 0; i < b->rows; i++) {
	t->data[r][c] += a->data[r][i] * b->data[i][c];
      }
    }
  }
  free_matrix_data(b);
  b->rows = t->rows;
  b->columns = t->columns;
  b->last_col = t->last_col;
  b->data = t->data;
  free(t);
}
void multiple(MATRIX * a, MATRIX * b){

}
void ident(MATRIX * m){
  for(int r = 0; r < m->columns; r++){
    for(int c = 0; c < m->columns; c++){
      m->data[r][c] = (r == c);
    }
  }
}

void scale(MATRIX * m, double x, double y, double z){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = x;
  t->data[1][1] = y;
  t->data[2][2] = z;
  multiply(t, m);
  free(t);
}
void project(MATRIX * m, double d){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = 1;
  t->data[1][1] = 1;
  t->data[2][2] = 0;  
  t->data[3][2] = - 1 / d;
  multiply(t, m);
  free(t);
}
void translate(MATRIX *m, double x, double y, double z){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][3] = x;
  t->data[1][3] = y;
  t->data[2][3] = z;
  multiply(t, m);
  free(t);
}
void rotate_z_axis(MATRIX *m, double theta){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = cos(theta);
  t->data[0][1] = -sin(theta);
  t->data[1][0] = sin(theta);
  t->data[1][1] = cos(theta);
  multiply(t, m);
  free(t);
}
void rotate_y_axis(MATRIX *m, double theta){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = cos(theta);
  t->data[0][2] = sin(theta);
  t->data[2][0] = -sin(theta);
  t->data[2][2] = cos(theta);
  multiply(t, m);
  free(t);
}
void rotate_x_axis(MATRIX *m, double theta){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[1][1] = cos(theta);
  t->data[1][2] = -sin(theta);
  t->data[2][1] = sin(theta);
  t->data[2][2] = cos(theta);
  multiply(t, m);
  free(t);
}
void rotate(MATRIX *m, double x_theta, double y_theta, double z_theta){
  rotate_x_axis(m, x_theta);
  rotate_y_axis(m, y_theta);
  rotate_z_axis(m, z_theta);
}
int factorial(int n){
  int res = 1;
  for(int i = 1; i <= n; i++){
    res *= i;
  }
  return res;
}
int binomial(int n, int k){
  return factorial(n) / (factorial(k) * factorial(n - k));
}
void circle(ELEMENT * e, double cx, double cy, double cz, double radius){
  int x, y, xn, yn;
  double t_inc = (2 * M_PI) / 360;
  for(double theta = 0; theta < 2 * M_PI; theta += t_inc){
    get_point_polar(cx, cy, theta, radius, &x, &y);
    get_point_polar(cx, cy, theta + t_inc, radius, &xn, &yn);
    add_line(e, x, y, cz, xn, yn, cz);
  }
  
}
void bezier(ELEMENT * e, int positions[], int degree, double t_inc){
  double t, x, y, z = 0;
  t = 0;
  double binomials[degree+ 1];
  for(int i = 0; i < degree + 1; i++){
    binomials[i] = binomial(degree, i);
  }
  add_col(e, positions[0], positions[1], positions[2]);
  while(t <= 1){
    x = 0;
    y = 0;
    z = 0;
    for(int i = 0; i < degree + 1; i++){
      double p1 = pow(1 - t, degree - i);
      double p2 = pow(t, i);
      x += binomials[i] * p1 * p2 * positions[3 * i];
      y += binomials[i] * p1 * p2 * positions[3 * i + 1];
      z += binomials[i] * p1 * p2 * positions[3 * i + 2];
    }
    add_col(e, x, y, z);
    add_col(e, x, y, z);
    t += t_inc;
  }
  add_col(e, positions[3 * (degree)],positions[3 * (degree) + 1],positions[3 * (degree) + 2]);
}
void hermite_basis(MATRIX * m){
  int d[] = {2, -2, 1, 1,
	     -3, 3, -2, -1,
	     0, 0, 1, 0,
	     1, 0, 0, 0};
  for(int i = 0; i < 16; i++){
    m->data[i / 4][i % 4] = d[i];
  }
}
void hermite_control(MATRIX * m, double data[]){
  for(int i = 0; i < 12; i++){
    m->data[i / 3][i % 3] = data[i];
  }
}
void hermite(ELEMENT * e, double data[], double t_inc){
  //data = {x1, y1, z1, x2, y2, z2, x'1, y'1, z'1, x'2, y'2, z'2}
  //data = {P1, P2, P'1, P'2}
  double t = 0;
  double x, y, z;
  add_col(e, data[0], data[1], data[3]);
  while(t <= 1){
    MATRIX * basis = generate_matrix(4, 4);
    MATRIX * control = generate_matrix(4, 3);
    MATRIX * s = generate_matrix(1, 4);
    hermite_basis(basis);
    hermite_control(control, data);
    s->data[0][0] = pow(t, 3);
    s->data[0][1] = pow(t, 2);
    s->data[0][2] = t;
    s->data[0][3] = 1;
    multiply(s, basis);
    multiply(basis, control);
    x = control->data[0][0];
    y = control->data[0][1];
    z = control->data[0][2];
    add_col(e, x, y, z);
    add_col(e, x, y, z);
    free_matrix(basis);
    free_matrix(control);
    free_matrix(s);
    t += t_inc;
  }
  add_col(e, x, y, z);

}

void clear(ELEMENT * e){
  MATRIX * m = e->matrix;
  for(int i = 0; i < m->rows; i++){
    memset(m->data[i], 0.0, sizeof(double) * m->columns);
  }
  e->length = 0;  
  m->last_col = 0;
  
}
void box(ELEMENT * e, double x, double y, double z, double width, double height, double depth){
  add_line(e, x, y, z, x + width, y, z);
  add_line(e, x, y, z, x, y - height, z);
  add_line(e, x + width, y - height, z, x + width, y, z);
  add_line(e, x + width, y - height, z, x, y - height, z);

  add_line(e, x, y, z, x, y, z - depth);
  add_line(e, x + width, y, z, x + width, y, z - depth);
  add_line(e, x, y - height, z, x, y - height, z - depth);
  add_line(e, x + width, y - height, z, x + width, y - height, z - depth);

  add_line(e, x, y, z - depth, x + width, y, z - depth);
  add_line(e, x, y, z - depth, x, y - height, z - depth);
  add_line(e, x + width, y - height, z - depth, x + width, y, z - depth);
  add_line(e, x + width, y - height, z - depth, x, y - height, z - depth);
}
ELEMENT * generate_sphere(double x, double y, double z, double r){
  ELEMENT * e = generate_element(102, rgb(255, 255, 255));
  double t_inc = M_PI / 60;
  for(double theta = 0; theta < 2 * M_PI; theta += t_inc){
    for(double phi = 0; phi < M_PI; phi += t_inc){
	add_col(e,
		x + (r * cos(theta) * sin(phi)),
		y + (r * sin(theta) * sin(phi)),
		z + (r * cos(phi)));
      }
  }
  return e;
}
void sphere(ELEMENT * e, double x, double y, double z, double r){
  ELEMENT * s = generate_sphere(x, y, z, r);
  MATRIX * m = s->matrix;
  for(int i = 0; i < s->length; i++){
    add_line(e,
	     m->data[0][i], m->data[1][i], m->data[2][i],
	     m->data[0][i] + 0, m->data[1][i] + 0, m->data[2][i] + 0);
  }
}
