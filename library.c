#include "library.h"
GRID * generate_grid(int width, int height){
  GRID * m = malloc(sizeof(GRID));
  int* values = calloc(width * height, sizeof(int));
  int** rows = malloc(height * sizeof(int*));
  for (int i=0; i<height; ++i){
    rows[i] = values + i * width;
  }
  m->data = rows;

  double * z_vals = calloc(width * height, sizeof(double));
  double** z_rows = malloc(height * sizeof(double*));
  for (int i=0; i<height; ++i){
    z_rows[i] = z_vals + i * width;
    memset(z_rows[i], -DBL_MAX, sizeof(double) * width);
  }
  m->z_buffer = z_rows;
  //memset(z_rows, -DBL_MAX, sizeof(double) * width * height);
  m->width = width;
  m->height = height;
  return m;
}
void plot(GRID * grid, int x, int y, double z, int rgb){
  if (y < 0 || x < 0 || x >= grid->width || y >= grid->height)
    return;
  //printf("(%d %d %f, %f)", x, y, z, grid->z_buffer[grid->height - 1 - y][x]);
  if(z > grid->z_buffer[grid->height - 1 - y][x]){
    grid->z_buffer[grid->height - 1 - y][x] = z;
    grid->data[grid->height - 1 - y][x] = rgb;
  }
  //printf("-->(%d %d %f, %f) \n", x, y, z, grid->z_buffer[grid->height - 1 - y][x]);
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
    for(int x = 0; x < grid->width; x++){
      grid->data[y][x] = rgb(255, 255, 255);
      grid->z_buffer[y][x] = -DBL_MAX;
    }
}
void draw_line_steep(GRID * grid, int x1, int y1, double z1, int x2, int y2, double z2, int rgb){
  if(y2 < y1){
    SWAP(x1, x2, int);
    SWAP(y1, y2, int);
    SWAP(z1, z2, double);
  }
  double delta_z = z2 - z1;
  int delta_y = y2 - y1;
  int delta_x = x2 - x1;
  int x_inc = delta_x/abs(delta_x);
  double z_inc = delta_z / delta_y;
  if(x_inc < 0)
    delta_x = -delta_x;
  int dx = 2 * delta_x - delta_y;
  while(y1 <= y2){
    plot(grid, x1, y1, z1, rgb);
    if(dx >= 0){
      x1 += x_inc;
      dx -= (2 * delta_y);
    }
    z1 += z_inc;
    y1 += 1;
    dx += (2 * delta_x);
  }
}
void draw_line_vertical(GRID * grid, int x1, int y1, double z1, int x2, int y2, double z2, int rgb){
  if(y2 < y1){
    SWAP(y1, y2, int);
    SWAP(z1, z2, double);
  }
  double delta_z = z2 - z1;
  double z_inc = delta_z /(y2 - y1);
  while(y1 <= y2){
    plot(grid, x1, y1, z1, rgb);
    z1 += z_inc;
    y1 += 1;
  }
}
void draw_line_gentle(GRID * grid, int x1, int y1, double z1, int x2, int y2, double z2, int rgb){
  if(x2 < x1){
    SWAP(x1, x2, int);
    SWAP(y1, y2, int);
    SWAP(z1, z2, double);
  }
  double delta_z = z2 - z1;
  int delta_y = y2 - y1;
  int delta_x = x2 - x1;
  int y_inc = delta_y ? delta_y/abs(delta_y) : 1;
  double z_inc = delta_z / delta_x;
  if(y_inc < 0)
    delta_y = -delta_y;
  int d = 2 * delta_y - delta_x;
  while(x1 <= x2){
    plot(grid, x1, y1, z1, rgb);
    if(d >= 0){
      y1 += y_inc;
      d -= (2 * delta_x);
    }
    x1 += 1;
    z1 += z_inc;
    d += (2 * delta_y);
  }
}
void draw_line(GRID * grid, int x1, int y1, double z1, int x2, int y2, double z2, int rgb){
  //printf("%-4d %-4d - %-4d %-4d \n", x1, y1, x2, y2);
  double slope = (x2 - x1) ? ((y2 - y1) * 1.0 / (x2 - x1)) : INT_MAX;
  if(fabs(slope) <= 1.0){
    draw_line_gentle(grid, x1, y1, z1, x2, y2, z2, rgb);
  }else if(slope == INT_MAX){
    draw_line_vertical(grid, x1, y1, z1, x2, y2, z2, rgb);
  }else{
    draw_line_steep(grid, x1, y1, z1, x2, y2, z2, rgb);
  }
}
void draw_line_polar(GRID * grid, int x1, int y1, double r, double theta, int rgb){
  draw_line(grid, x1, y1, 0, x1 + (int) (r * cos(theta)), y1 + (int) (r * sin(theta)), 0, rgb);
}

VECTOR generate_vector(double x, double y, double z){
  VECTOR v = calloc(sizeof(double) * 3, 1);
  v[0] = x;
  v[1] = y;
  v[2] = z;
  return v;
}
double dot_product(VECTOR a, VECTOR b){
  return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}
VECTOR calculate_normal(MATRIX * matrix, int i){
  double **m = matrix->data;
  VECTOR a = generate_vector(m[0][i + 1] - m[0][i],
			     m[1][i + 1] - m[1][i],
			     m[2][i + 1] - m[2][i]);
  VECTOR b = generate_vector(m[0][i + 2] - m[0][i],
			     m[1][i + 2] - m[1][i],
			     m[2][i + 2] - m[2][i]);
  VECTOR n = generate_vector((a[1] * b[2]) - (a[2] * b[1]),
			     (a[2] * b[0]) - (a[0] * b[2]),
			     (a[0] * b[1]) - (a[1] * b[0]));
  free(a);
  free(b);
  return n;
}


ELEMENT * generate_element(int size, int color){
  ELEMENT *e = (ELEMENT *)malloc(sizeof(ELEMENT));
  e->edge_matrix = generate_matrix(4, size * 2);
  e->triangle_matrix = generate_matrix(4, size * 3);
  e->color = color;
  e->length = 0;
  e->triangle_length = 0;
  e->next_element = NULL;
  e->children = NULL;
  return e;
}
void add_col(ELEMENT  * e, int x, int y, int z, int polygon){
  MATRIX * M = polygon ? e->triangle_matrix : e->edge_matrix;
  double ** m = M->data;
  int l = polygon ? e->triangle_length : e->length;
  m[0][l] = x;
  m[1][l] = y;
  m[2][l] = z;
  m[3][l] = 1;
  int t = polygon ? (e->triangle_length += 1) : (e->length += 1);
  int nl = (M->columns * 3)/2;
  if(l + 1 == M->columns)
    grow_matrix(M, (M->columns * 3)/2);
}
void copy_last_col(ELEMENT * e){
  double ** m = e->edge_matrix->data;
  int l = e->length - 1;
  add_col(e, m[0][l], m[1][l], m[2][l], 0);
}
void add_line(ELEMENT * e, int x, int y, int z, int x2, int y2, int z2){
  add_col(e, x, y, z, 0);
  add_col(e, x2, y2, z2, 0);
}
void add_triangle(ELEMENT * e, int x, int y, int z, int x2, int y2, int z2, int x3, int y3, int z3){
  add_col(e, x, y, z, 1);
  add_col(e, x2, y2, z2, 1);
  add_col(e, x3, y3, z3, 1);
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

void draw_scanline(GRID * g, int x1, int x2, int y, double z1, double z2, int color){
  double dz = (z2 - z1) / (x2 - x1);
  if(x1 > x2)
    SWAP(x1, x2, int);
  while(x1 < x2){
    plot(g, x1, y, z1, color);
    z1 += dz;
    x1++;
  }
}

void draw_triangle(ELEMENT * e, GRID * g, int c){
  MATRIX * m = e->triangle_matrix;
  //Find the top bottom and middle of the triangle
  int top = c, mid = c + 1, bot = c + 2;
  if(m->data[1][top] < m->data[1][mid])
    SWAP(top, mid, int);
  if(m->data[1][mid] < m->data[1][bot])
    SWAP(mid, bot, int);
  if(m->data[1][top] < m->data[1][mid])
    SWAP(top, mid, int);
  if(m->data[1][top] == m->data[1][mid] && m->data[0][top] > m->data[0][mid])
    SWAP(top, mid, int);
  if(m->data[1][mid] == m->data[1][bot] && m->data[0][mid] < m->data[0][bot])
    SWAP(mid, bot, int);

  //Setup the variables for the loop
  int y = m->data[1][bot];
  int y_mid = m->data[1][mid];
  int y_top = m->data[1][top];

  double x1 = m->data[0][bot];
  double x2 = m->data[0][bot];

  double z1 = m->data[2][bot];
  double z2 = m->data[2][bot];

  //Flat bottom
  if(y == y_mid){
    x2 = m->data[0][mid];
    z2 = m->data[2][mid];
  }
  //Get rates of change for the various sides of the triangle
  double dx_0 = (m->data[0][top] - m->data[0][bot]) / (y_top - y);
  double dx_1 = (m->data[0][mid] - m->data[0][bot]) / (y_mid - y);
  double dx_2 = (m->data[0][top] - m->data[0][mid]) / (y_top - y_mid);

  double dz_0 = (m->data[2][top] - m->data[2][bot]) / (y_top - y);
  double dz_1 = (m->data[2][mid] - m->data[2][bot]) / (y_mid - y);
  double dz_2 = (m->data[2][top] - m->data[2][mid]) / (y_top - y_mid);
  /* printf("%d -- (%d %d %d) (%d %d %d) (%d %d %d) \n", c, */
  /* 	 (int) m->data[0][top], (int) m->data[1][top], (int) m->data[2][top], */
  /* 	 (int) m->data[0][mid], (int) m->data[1][mid], (int) m->data[2][mid], */
  /* 	 (int) m->data[0][bot], (int) m->data[1][bot], (int) m->data[2][bot] */
  /* 	 ); */

  if(!(y_top - y)){
    dx_0 = 0;

  }
  while(y < y_mid){
    draw_scanline(g, (int) x1, (int) x2, y, z1, z2, e->color);
    x1 += dx_0;
    x2 += dx_1;
    z1 += dz_0;
    z2 += dz_1;
    y += 1;
  }
  while(y <= y_top){
    draw_scanline(g, (int) x1, (int) x2, y, z1, z2, e->color);
    x1 += dx_0;
    x2 += dx_2;
    z1 += dz_0;
    z2 += dz_2;
    y += 1;
  }
}
void plot_element(ELEMENT * e, GRID * g){
  VECTOR viewpoint = generate_vector(0, 0, 1);
  if(!e)
    return;
  MATRIX *m = e->edge_matrix;
  for(int c = 0; c < m->columns; c += 2){
    draw_line(g, (int)m->data[0][c], (int)m->data[1][c], m->data[2][c],
	      (int)m->data[0][c + 1], (int)m->data[1][c + 1], m->data[2][c + 1],
	      e->color);
  }
  m = e->triangle_matrix;
  for(int c = 0; c < e->triangle_length; c += 3){
    VECTOR n = calculate_normal(m, c);
    if(dot_product(viewpoint, n) >= 0){

      e->color = rgb(rand() % 256, rand() % 256, rand() % 256);
      draw_triangle(e, g, c);
      draw_line(g, (int)m->data[0][c], (int)m->data[1][c], m->data[2][c],
	      (int)m->data[0][c + 1], (int)m->data[1][c + 1], m->data[2][c + 1],
	      e->color);
      draw_line(g, (int)m->data[0][c], (int)m->data[1][c], m->data[2][c],
	      (int)m->data[0][c + 2], (int)m->data[1][c + 2], m->data[2][c + 2],
	      e->color);
      draw_line(g, (int)m->data[0][c + 1], (int)m->data[1][c + 1], m->data[2][c + 1],
		(int)m->data[0][c + 2], (int)m->data[1][c + 2], m->data[2][c + 2],
		e->color);
    }
    free(n);
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
  m->next = NULL;
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

MATRIX * push_to_stack(MATRIX * m){
  MATRIX * new = generate_matrix(4, 4);
  copy_matrix(m, new);
  new->next = m;
  return new;
}
MATRIX * pop_from_stack(MATRIX * m){
  MATRIX * next = m->next;
  free_matrix(m);
  return next;
}
void transform_stack(MATRIX * m, MATRIX * t){
  multiply(m, t);
  copy_matrix(t, m);
  //free(t);
}

void speckle(ELEMENT * e, int x, int y, int z, int width, int height, int depth, int density , int radius, int spiked){
  width += x;
  height += y;
  depth += z;
  radius += 1;
  printf("%d %d %d\n", x ,y, z);
  printf("%d %d %d\n", width ,height, depth);
  for(int x2 = x; x2 < width; x2++){
    for(int y2 = y; y2 < height; y2++){
      for(int z2 = z; z2 < depth; z2++){
	if(rand() % 100 < density){
	  int r = (rand() % radius) + 1;

	  if(spiked){
	    for(int i = 0; i < 5; i++){
	      add_line(e,
		       x2, y2, z2,
		       x2 + (rand() % (2 * r)) - r, y2 + (rand() % (2 * r)) - r, z2 + (rand() % (2 * r)) - r);
	    }
	  }else{
	    add_line(e, x2, y2, z2, x2, y2, z2);
	  }
	}
      }
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
  add_col(e, positions[0], positions[1], positions[2], 0);
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
    add_col(e, x, y, z, 0);
    add_col(e, x, y, z, 0);
    t += t_inc;
  }
  add_col(e, positions[3 * (degree)],positions[3 * (degree) + 1],positions[3 * (degree) + 2], 0);
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
  add_col(e, data[0], data[1], data[3], 0);
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
    add_col(e, x, y, z, 0);
    add_col(e, x, y, z, 0);
    free_matrix(basis);
    free_matrix(control);
    free_matrix(s);
    t += t_inc;
  }
  add_col(e, x, y, z, 0);

}

void clear(ELEMENT * e){
  MATRIX * m = e->edge_matrix;
  for(int i = 0; i < m->rows; i++){
    memset(m->data[i], 0.0, sizeof(double) * m->columns);
  }
  e->length = 0;
  m->last_col = 0;


  m = e->triangle_matrix;
  //print_matrix(m);
  for(int i = 0; i < m->rows; i++){
    //free(m->data[i]);
    //m->data[i] = (double *)calloc(m->columns * sizeof(double), 1);
    memset(m->data[i], 0.0, sizeof(double) * m->columns);
  }
  //print_matrix(m);
  m->last_col = 0;
  e->triangle_length = 0;

}
void box(ELEMENT * e, double x, double y, double z, double width, double height, double depth){
  //Very bad C practice
  double points[8][4] = {
		       {x, y, z}, {x, y - height, z}, {x + width, y - height, z}, {x + width, y, z},
		       {x, y, z - depth}, {x, y - height, z - depth}, {x + width, y - height, z - depth}, {x + width, y, z - depth}};
  #define t(p1, p2, p3, p4) ({						\
      add_triangle(e,							\
		   points[p1][0], points[p1][1], points[p1][2],		\
		   points[p2][0], points[p2][1], points[p2][2],		\
		   points[p3][0], points[p3][1], points[p3][2]);	\
      add_triangle(e,							\
		   points[p3][0], points[p3][1], points[p3][2],		\
		   points[p4][0], points[p4][1], points[p4][2],		\
		   points[p1][0], points[p1][1], points[p1][2]);})

  t(0, 1, 2, 3);
  t(0, 3, 7, 4);
  t(3, 2, 6, 7);
  t(2, 1, 5, 6);
  t(1, 0, 4, 5);
  t(7, 6, 5, 4);
  #undef t
}
ELEMENT * generate_sphere(double x, double y, double z, double r){
  int res = 20;
  ELEMENT * e = generate_element(res * res + 1, rgb(255, 255, 255));
  double t_inc = M_PI / res;
  double p_inc = M_PI / (res - 1);
  for(int t = 0; t < 2 * res; t++){
    for(int p = 0; p < res; p++){
      double theta = t * t_inc;
      double phi = p * p_inc;
      add_col(e,
	      x + (r * cos(theta) * sin(phi)),
	      y + (r * sin(theta) * sin(phi)),
	      z + (r * cos(phi)), 0);
      }
  }
  return e;
}
void sphere(ELEMENT * e, double x, double y, double z, double r){
  ELEMENT * s = generate_sphere(x, y, z, r);
  MATRIX * m = s->edge_matrix;
  int res = 20;
  for(int i = 0; i < s->length; i++){
    int a = (i + 1);
    int t = (i + res) % (2 * res * res);
    int q = (a + res) % (2 * res * res);
    if(i % res == res - 1)
      continue;
    if((i % res))
      add_triangle(e,
		   m->data[0][i], m->data[1][i], m->data[2][i],
		   m->data[0][t], m->data[1][t], m->data[2][t],
		   m->data[0][a], m->data[1][a], m->data[2][a]);
    if(!(i % res == (res - 2)))
      add_triangle(e,
		   m->data[0][t], m->data[1][t], m->data[2][t],
		   m->data[0][q], m->data[1][q], m->data[2][q],
		   m->data[0][a], m->data[1][a], m->data[2][a]);
  }

  free_matrix(m);
}

ELEMENT * generate_torus(double x, double y, double z, double R, double r){
  ELEMENT * e = generate_element(102, rgb(255, 255, 255));
  int res = 20;
  double t_inc = M_PI / res;
  for(int t = 0; t < 2 * res; t++){
    for(int p = 0; p < 2 * res; p++){
      double theta = t * t_inc;
      double phi = p * t_inc;
      add_col(e,
	      x + (cos(theta) * (r * cos(phi) + R)),
	      y + (r * sin(phi)),
	      z - (sin(theta) * (r * cos(phi) + R)), 0);

      }
  }
  return e;
}
void torus(ELEMENT * e, double x, double y, double z, double r, double R){
  ELEMENT * s = generate_torus(x, y, z, R, r);
  MATRIX * m = s->edge_matrix;
  int res = 20;
  for(int i = 0; i < s->length; i++){
    /* T - I
       | X |
       Q - A
     */
    int a = i + 1;
    a = (a % (2 * res)) ? a : a - 2 * res;
    int t = (i + 2 * res) % s->length;
    int q = (a + 2 * res) % s->length;
    add_triangle(e,
		 m->data[0][i], m->data[1][i], m->data[2][i],
		 m->data[0][t], m->data[1][t], m->data[2][t],
		 m->data[0][a], m->data[1][a], m->data[2][a]);
    add_triangle(e,
		 m->data[0][q], m->data[1][q], m->data[2][q],
		 m->data[0][a], m->data[1][a], m->data[2][a],
		 m->data[0][t], m->data[1][t], m->data[2][t]);
  }
  free_matrix(m);
}
