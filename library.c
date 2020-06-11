#include "library.h"
GRID * generate_grid(int width, int height){
  GRID * m = malloc(sizeof(GRID));

  int* values = calloc(width * height, sizeof(int));//Allows you to free the whole matrix/2d_array at once
  int** rows = malloc(height * sizeof(int*));
  for (int i=0; i<height; ++i){
    rows[i] = values + i * width;
  }
  m->data = rows;

  double * z_vals = calloc(width * height, sizeof(double));
  double** z_rows = malloc(height * sizeof(double*));
  memset(z_vals, -DBL_MAX, sizeof(double) * height * width);
  for (int i=0; i<height; ++i){
    z_rows[i] = z_vals + i * width;
  }
  m->z_buffer = z_rows;

  m->width = width;
  m->height = height;
  return m;
}
void free_grid(GRID * g){
  free(*g->data);
  free(g->data);
  free(*g->z_buffer);
  free(g->z_buffer);

  free(g);
}
void plot(GRID * grid, int x, int y, double z, int rgb){
  if (y < 0 || x < 0 || x >= grid->width || y >= grid->height)
    return;
  //printf("(%d %d %f, %f)", x, y, z, grid->z_buffer[grid->height - 1 - y][x]);
  if(z >= grid->z_buffer[grid->height - 1 - y][x]){
    grid->z_buffer[grid->height - 1 - y][x] = z;
    grid->data[grid->height - 1 - y][x] = rgb;
  }
  //printf("-->(%d %d %f, %f) \n", x, y, z, grid->z_buffer[grid->height - 1 - y][x]);
}
int rgb(int r, int g, int b){
  if(r < 0)
    return r;
  return((r&0x0ff)<<16)|((g&0x0ff)<<8)|(b&0x0ff);
}
double distance(int x1, int y1, int x2, int y2){
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
double degrees_to_radians(double degrees){
  return (M_PI * degrees) / 180.0;
}
int radians_to_degrees(double radians){
  return (int) (180 * radians / M_PI);
}
void get_point_polar(int x, int y, double theta, double r, int *xn, int *yn){
  *xn = x + (r * cos(theta));
  *yn = y + (r * sin(theta));
}
void get_point_polar_3d(double *in_buf, double theta, double phi, double r, double *buf){
  *buf = *in_buf + (r * sin(phi) * cos(theta));
  *(buf + 1) = *(in_buf + 1) + (r * sin(phi) * sin(theta));
  *(buf + 2) = *(in_buf + 2) + (r * cos(phi));
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
  return 0;
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
    SWAP(x1, x2, int);
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
VECTOR subtract(VECTOR a, VECTOR b){
  return generate_vector(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}
VECTOR scale_vector(VECTOR v, double d){
  return generate_vector(v[0] * d, v[1] * d, v[2] * d);
}
double dot_product(VECTOR a, VECTOR b){
  return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}
VECTOR cross_product(VECTOR a, VECTOR b){
  return generate_vector((a[1] * b[2]) - (a[2] * b[1]),
			     (a[2] * b[0]) - (a[0] * b[2]),
			     (a[0] * b[1]) - (a[1] * b[0]));
}
double magnitude(VECTOR v){
  return sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}
VECTOR normalized(VECTOR v){
  double m = magnitude(v);
  return generate_vector(v[0] / m, v[1] / m, v[2] / m);
}
VECTOR calculate_surface_normal(MATRIX * matrix, int i){
  double **m = matrix->data;
  VECTOR a = generate_vector(m[0][i + 1] - m[0][i],
			     m[1][i + 1] - m[1][i],
			     m[2][i + 1] - m[2][i]);
  VECTOR b = generate_vector(m[0][i + 2] - m[0][i],
			     m[1][i + 2] - m[1][i],
			     m[2][i + 2] - m[2][i]);
  VECTOR n = cross_product(a, b);
  free(a);
  free(b);
  VECTOR v = normalized(n);
  free(n);
  return v;
}
VECTOR split_rgb(int rgb){
  return generate_vector((rgb >> 16) &0x0ff, (rgb >> 8) &0x0ff, rgb &0x0ff);
}

LIGHT * generate_light(double x, double y, double z, int r, int g, int b){
  LIGHT * l = malloc(sizeof(LIGHT));
  VECTOR v = generate_vector(x, y, z);
  l->vector = normalized(v);
  free(v);
  l->rgb = generate_vector(r, g, b);
  return l;
}
void free_light(LIGHT * l){
  if(!l)
    return;
  free(l->vector);
  free(l->rgb);
  free(l);
}
void add_light(ELEMENT * e, char * name, int x, int y, int z, int r, int g, int b){
  LIGHT ** old_light = map_get(&e->lights, name);
  if(old_light)
    free_light(*old_light);
  LIGHT * l = generate_light(x, y, z, r, g, b);
  map_set(&e->lights, name, l);
}
void set_texture(ELEMENT * e, double ac, double dc, double sc){
  for(int i = 0; i < 3; i++){
    e->ambient_const[i] = ac;
    e->diffuse_const[i] = dc;
    e->specular_const[i] = sc;
  }
}
void set_texture_rgb(ELEMENT * e, double ac_r, double ac_g, double ac_b,
		     double dc_r, double dc_g, double dc_b,
		     double sc_r, double sc_g, double sc_b){
  free(e->ambient_const);
  e->ambient_const = generate_vector(ac_r, ac_g, ac_b);
  free(e->diffuse_const);
  e->diffuse_const = generate_vector(dc_r, dc_g, dc_b);
  free(e->specular_const);
  e->specular_const = generate_vector(sc_r, sc_g, sc_b);
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
  e->ambient_const = generate_vector(.2, .2, .2);
  e->diffuse_const = generate_vector(.5, .5, .5);
  e->specular_const = generate_vector(.5, .5, .5);
  map_init(&e->lights);
  e->viewpoint = generate_vector(0, 0, 1);
  //add_light(e, 0, 0, 1, 255, 255, 255);
  return e;
}
void free_element(ELEMENT * e){
  free_matrix(e->edge_matrix);
  free_matrix(e->triangle_matrix);
  free(e->ambient_const);
  free(e->diffuse_const);
  free(e->specular_const);
  free(e->viewpoint);
  map_iter_t iter = map_iter(&e->lights);
  char * key;
  while((key = map_next(&e->lights, &iter))){
    LIGHT * res = *map_get(&e->lights, key);
    free_light(res);
  }
  map_deinit(&e->lights);
  free(e);
}
int calculate_color(ELEMENT * e, VECTOR normal){
  VECTOR viewpoint = e->viewpoint;
  int color = e->color;
  if(e->color == -2)
    color = rgb(rand() % 256, rand() % 256, rand() % 256);

  VECTOR col = split_rgb(color);
  VECTOR rgb_vec = generate_vector(col[0], col[1], col[2]);
  for(int i = 0; i < 3; i++)
    rgb_vec[i] *= e->ambient_const[i];
  //I = (A * Ka) + (P * Kd * (N̂ • L̂)) + (P * Kd * [(2N̂(N̂ • L̂) - L̂) • V̂]ⁿ)
  //LIGHT * light;
  char * key;
  map_iter_t iter = map_iter(&e->lights);
  //LIGHT * light = e->lights;
  while((key = map_next(&e->lights, &iter))){
    LIGHT * light = *map_get(&e->lights, key);
    double d = dot_product(normal, light->vector);
    VECTOR scaled = scale_vector(normal, d * 2);
    VECTOR subtracted = subtract(scaled, light->vector);
    for(int i = 0; i < 3; i++){
      rgb_vec[i] += fmax(0, light->rgb[i] * e->diffuse_const[i] * d) +
	fmax(0, light->rgb[i] * e->specular_const[i] * pow(dot_product(subtracted, viewpoint) , 4));
    }
    free(scaled);
    free(subtracted);
  }
  for(int i = 0; i < 3; i++){
    rgb_vec[i] = fmin(255, rgb_vec[i]);
    rgb_vec[i] = fmax(0, rgb_vec[i]);
  }
  int rgb_val = rgb(rgb_vec[0], rgb_vec[1], rgb_vec[2]);
  free(col);
  free(rgb_vec);
  return rgb_val;
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
void add_quadrilateral(ELEMENT * e, int x, int y, int z, int x2, int y2, int z2, int x3, int y3, int z3, int x4, int y4, int z4){
  // 2   1
  // |   |
  // 3 - 4
  add_triangle(e, x, y, z, x2, y2, z2, x3, y3, z3);
  add_triangle(e, x3, y3, z3, x4, y4, z4, x, y, z);
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

void set_color(ELEMENT * e, int color){
  e->color = color;
}
void draw_scanline(GRID * g, int x1, int x2, int y, double z1, double z2, int color){
  if(x1 > x2){
    SWAP(x1, x2, int);
    SWAP(z2, z1, double);
  }
  double dz = (z2 - z1) / (x2 - x1);
  while(x1 < x2){
    plot(g, x1, y, z1, color);
    z1 += dz;
    x1++;
  }
}

void draw_triangle(ELEMENT * e, GRID * g, int c, int color){
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

  //Flat bottom, unlike mine
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
  /* printf("%d -- (%d %d %d) (%d %d %d) (%d %d %d)  -  %f %f  - %f %f %f\n", c, */
  /* 	 (int) m->data[0][top], (int) m->data[1][top], (int) m->data[2][top], */
  /* 	 (int) m->data[0][mid], (int) m->data[1][mid], (int) m->data[2][mid], */
  /* 	 (int) m->data[0][bot], (int) m->data[1][bot], (int) m->data[2][bot], */
  /* 	 z1, z2, dz_0, dz_1, dz_2); */

  if(!(y_top - y)){
    dx_0 = 0;
  }
  while(y < y_mid){
    draw_scanline(g, (int) x1, (int) x2, y, z1, z2, color);
    x1 += dx_0;
    x2 += dx_1;
    z1 += dz_0;
    z2 += dz_1;
    y += 1;
  }
  while(y <= y_top){
    draw_scanline(g, (int) x1, (int) x2, y, z1, z2, color);
    x1 += dx_0;
    x2 += dx_2;
    z1 += dz_0;
    z2 += dz_2;
    y += 1;
  }
}
void plot_element(ELEMENT * e, GRID * g){
  if(!e)
    return;
  MATRIX *m = e->edge_matrix;
  int color = e->color;// -1 => wireframe, -2 = random
  if(e->color == -2)
    color = rgb(rand() % 256, rand() % 256, rand() % 256);
  for(int c = 0; c < m->columns; c += 2){
    draw_line(g, (int)m->data[0][c], (int)m->data[1][c], m->data[2][c],
	      (int)m->data[0][c + 1], (int)m->data[1][c + 1], m->data[2][c + 1],
	      color);
  }
  m = e->triangle_matrix;
  for(int c = 0; c < e->triangle_length; c += 3){
    VECTOR n = calculate_surface_normal(m, c);
    if(dot_product(e->viewpoint, n) > 0){
      if(e->color == -2)
	color = rgb(rand() % 256, rand() % 256, rand() % 256);
      if(e->color != -1){
	draw_triangle(e, g, c,  calculate_color(e, n));
      }else{
	color = rgb(0, 0, 0);
	draw_line(g, (int)m->data[0][c], (int)m->data[1][c], m->data[2][c],
		  (int)m->data[0][c + 1], (int)m->data[1][c + 1], m->data[2][c + 1],
		  color);
	draw_line(g, (int)m->data[0][c], (int)m->data[1][c], m->data[2][c],
		  (int)m->data[0][c + 2], (int)m->data[1][c + 2], m->data[2][c + 2],
		  color);
	draw_line(g, (int)m->data[0][c + 1], (int)m->data[1][c + 1], m->data[2][c + 1],
		  (int)m->data[0][c + 2], (int)m->data[1][c + 2], m->data[2][c + 2],
		  color);
      }
    }
    free(n);
  }
  plot_element(e->next_element, g);
  plot_element(e->children, g);

}

MATRIX * generate_matrix(int rows, int cols){
  MATRIX *m = (MATRIX *)calloc(sizeof(MATRIX), 1);

  double **data = calloc(rows * sizeof(double *), 1);
  double * vals = calloc(rows * cols, sizeof(double));
  for (int i = 0;i < rows; i++) {
    data[i] = vals + (i * cols);
  }
  m->data=data;
  m->rows = rows;
  m->columns = cols;
  m->last_col = 0;
  m->next = NULL;
  return m;
}
void free_matrix(MATRIX * m){
  free(*m->data);
  free(m->data);
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
  double * old_data = *m->data;
  double * new_data = calloc(newcols * m->rows, sizeof(double));
  for (int i = 0; i < m->rows; i++) {
    memcpy(new_data + (i * newcols), m->data[i], sizeof(double) * m->columns);
    m->data[i] = new_data + (i * newcols);
  }
  free(old_data);
  m->columns = newcols;
}
void copy_matrix(MATRIX * a, MATRIX * b){

  for (int r=0; r < a->rows; r++)
    for (int c=0; c < a->columns; c++)
      b->data[r][c] = a->data[r][c];
}
void multiply(MATRIX * a, MATRIX * b){
  //https://stackoverflow.com/questions/1907557/optimized-matrix-multiplication-in-c
  //https://akkadia.org/drepper/cpumemory.pdf
  MATRIX * temp = generate_matrix(b->columns, a->rows);
  for (int r = 0; r < temp->rows; r++){
    for (int c = 0; c < temp->columns; c++){
      temp->data[r][c] = b->data[c][r];
    }
  }
  MATRIX * res = generate_matrix(a->rows, b->columns);
  for (int r=0; r < a->rows; r++){
    for (int c=0; c < b->columns; c++){
      for(int i = 0; i < b->rows; i++) {
	res->data[r][c] += a->data[r][i] * temp->data[c][i];
      }
    }
  }
  free(*b->data);
  free(b->data);
  free_matrix(temp);
  b->rows = res->rows;
  b->columns = res->columns;
  b->last_col = res->last_col;
  b->data = res->data;
  free(res);
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
}

double * generate_cone(int x, int y, int z, double theta, double phi, double inner_angle, int radius, int res){
  //https://math.stackexchange.com/questions/643130/circle-on-sphere
  // radius is the length on the surface on the cone
  // returns  data in format{origin_x, origin_y, origin, z, center_x, center_y, center_z, x1, y1, z1, ..., xn, yn, zn
  double * data = calloc(sizeof(double) * 3 * (res + 3) + 1, 1);
  double origin[] = {(double) x, (double) y, (double) z};
  double midpoint[3]; //Center of the base of the cone
  get_point_polar_3d(origin, theta, phi, radius * cos(inner_angle / 2), midpoint);
  memcpy(data, origin, 3 * sizeof(double));
  memcpy(data + 3, midpoint, 3 * sizeof(double));
  double inc = 2 * M_PI / res;

  double ci = cos(inner_angle);
  double si = sin(inner_angle);
  double ct = cos(theta);
  double st = sin(theta);
  double cp = cos(phi);
  double sp = sin(phi);

  for(int i = 0; i <= res; i++){
    double x1 = x + radius * ((si * ct * cp * cos(inc * i)) -
			      (si * sp * sin(inc * i)) +
			      (ci * st * cp));
    double y1 = y + radius * ((si * ct * sp * cos(inc * i)) +
			      (si * cp * sin(inc * i)) +
			      (ci * st * sp));
    double z1 = z + radius * ((si * st * cos(inc * i)) +
			      (ci * ct));
    double temp[] = {x1, y1, z1};
    memcpy(data + ((i + 2) * 3), temp, 3 * sizeof(double));
  }
  return data;
}
void cone(ELEMENT * e, int x, int y, int z, double theta, double phi, double inner_angle, int radius, int res, int closed){
  double * data = generate_cone(x, y, z, theta, phi, inner_angle, radius, res);
  for(int i = 6; i < (res + 2) * 3; i += 3){
    add_triangle(e, (int) data[0], (int) data[1], (int) data[2],
		 (int) data[i], (int) data[i + 1], (int) data[i + 2],
		 (int) data[i + 3], (int) data[i + 4], (int) data[i + 5]);
    if(closed){
      add_triangle(e, (int) data[3], (int) data[4], (int) data[5],
		   (int) data[i], (int) data[i + 1], (int) data[i + 2],
		   (int) data[i + 3], (int) data[i + 4], (int) data[i + 5]);
    }else{
      add_triangle(e, (int) data[i + 3], (int) data[i + 4], (int) data[i + 5],
		   (int) data[i], (int) data[i + 1], (int) data[i + 2],
		   (int) data[0], (int) data[1], (int) data[2]);
    }

  }
  free(data);
}
void speckle(ELEMENT * e, int x, int y, int z, int width, int height, int depth, int density , int radius, int spiked){
  width += x;
  height += y;
  depth += z;
  radius += 1;
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
void flower(ELEMENT * e, int x, int y, int z, int theta, int phi, int variance,	 int length, int tendrils, int bud){
  //Contains the information for each tendril: last_x, last_y, last_z, x, y, z, theta, phi
  double data[8 * tendrils];
  //Fill the tendrils with default information
  double template[] = {x, y, z, x, y, z, degrees_to_radians(theta), degrees_to_radians(phi)};
  for(int i = 0; i < (8 * tendrils); i += 8){
    memcpy(data + i, template, 8 * sizeof(double));
  }
  //Loop through the tendrils 'length' times, growing the stalks
  for(int i = 0; i < 2 * length; i++){
    for(int t = 0; t < 8 * tendrils; t += 8){
      for(int p = t; p < t + 3; p ++){
	data[p] = data[p + 3];
      }
      data[t + 6] += degrees_to_radians((rand() % (2 * variance)) - variance);
      data[t + 7] += degrees_to_radians((rand() % (2 * variance)) - variance);
      get_point_polar_3d(data + t, *(data + t + 6), *(data + t + 7), 2, data + t + 3);
      add_line(e,
	       (int) data[t + 0], (int) data[t + 1], (int) data[t + 2],
	       (int) data[t + 3], (int) data[t + 4], (int) data[t + 5]);
    }
  }
  if(bud){
    for(int t = 0; t < 8 * tendrils; t += 8){
      int petals = (rand() % 10) + 3;
      cone(e, (int)data[t + 3], (int)data[t + 4], (int)data[t + 5], data[t + 6], data[t + 7], M_PI / 4, 20, petals, 0);
    }
  }
}
void tendril(ELEMENT * e, int x, int y, int z, double theta, double phi, int variance, int length, int radius, int end, int res){
  //The different kinds of endings / tails these things can have
  enum tendril_end{Sphere, Cone, Tapered_Cone};
  //Some useful variables
  int pulsate = 0;
  int og_cone_radius = radius / sin(M_PI / 4);
  int cone_radius = radius / sin(M_PI / 4);
  double og_int_angle = M_PI / 4;
  double int_angle = M_PI / 4;
  double int_angle_decrease = int_angle / res;

  //Represent the direction of the face of each segment and the previous one: last_theta, last_phi, theta, phi
  double angles[] = {degrees_to_radians(theta), degrees_to_radians(phi), degrees_to_radians(theta), degrees_to_radians(phi)};

  // Hold the current segment and the last one
  double * last_pos = 0;
  double *pos = generate_cone(x, y, z, angles[0], angles[1], int_angle, cone_radius, res);

  // Close off the base of the tendril
  for(int p = 6; p < (res + 2) * 3; p += 3){
    add_triangle(e, pos[p], pos[p + 1], pos[p + 2],
		 pos[p + 3], pos[p + 4], pos[p + 5],
		 pos[3], pos[4], pos[5]);
  }

  //Grow the tendril
  int complete = 0;
  for(int i = 0; (i < length + res) && !complete; i++){
    if(last_pos)
      free(last_pos);
    last_pos = pos;
    memcpy(angles, angles + 2, 2 * sizeof(double));
    angles[2] += degrees_to_radians((rand() % (2 * variance)) - variance);
    angles[3] += degrees_to_radians((rand() % (2 * variance)) - variance);
    if(pulsate){
	cone_radius = og_cone_radius + (og_cone_radius * (sin(degrees_to_radians(((rand() % 30) + 45) * i))) / 2);
    }
    pos = generate_cone(pos[3], pos[4], pos[5], angles[2], angles[3], int_angle, cone_radius, res);

    if(i > length - 1){
      switch(end){
	case Sphere:
	  if(i == length){
	    memcpy(last_pos, last_pos + 3, 3 * sizeof(double));
	    int_angle = M_PI / 2;
	    int_angle_decrease = (M_PI / res);
	  }
	  int_angle -= int_angle_decrease;
	  free(pos);
	  pos = generate_cone(last_pos[0], last_pos[1], last_pos[2], angles[2], angles[3], int_angle, radius, res);
	  break;
	case Cone:
	  for(int p = 6; p < (res + 2) * 3; p += 3){
	    add_triangle(e, last_pos[p], last_pos[p + 1], last_pos[p + 2],
			 last_pos[p + 3], last_pos[p + 4], last_pos[p + 5],
			 pos[3], pos[4], pos[5]);
	  }
	  complete = 1;
	  break;
	case Tapered_Cone:
	  if(i == length){
	    memcpy(last_pos, last_pos + 3, 3 * sizeof(double));
	    int_angle -= int_angle_decrease;
	  }
	  int_angle -= int_angle_decrease;
	  free(pos);
	  pos = generate_cone(last_pos[0], last_pos[1], last_pos[2], angles[2], angles[3], int_angle, radius, res);
	  break;
      }
    }
    if(!complete){
      //Use the two cones given as rings, create another segment of the tendril by joining them
      for(int p = 6; p < (res + 2) * 3; p += 3){
	add_quadrilateral(e, pos[p], pos[p + 1], pos[p + 2],
			  pos[p + 3], pos[p + 4], pos[p + 5],
			  last_pos[p + 3], last_pos[p + 4], last_pos[p + 5],
			  last_pos[p], last_pos[p + 1], last_pos[p + 2]);
      }
    }
  }
  free(last_pos);
  free(pos);
}

void scale(MATRIX * m, double x, double y, double z){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = x;
  t->data[1][1] = y;
  t->data[2][2] = z;
  multiply(t, m);
  free_matrix(t);
}
void project(MATRIX * m, double d){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = 1;
  t->data[1][1] = 1;
  t->data[2][2] = 0;
  t->data[3][2] = - 1 / d;
  multiply(t, m);
  free_matrix(t);
}
void translate(MATRIX *m, double x, double y, double z){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][3] = x;
  t->data[1][3] = y;
  t->data[2][3] = z;
  multiply(t, m);
  free_matrix(t);
}
void rotate_z_axis(MATRIX *m, double theta){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = cos(theta);
  t->data[0][1] = -sin(theta);
  t->data[1][0] = sin(theta);
  t->data[1][1] = cos(theta);
  multiply(t, m);
  free_matrix(t);
}
void rotate_y_axis(MATRIX *m, double theta){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[0][0] = cos(theta);
  t->data[0][2] = sin(theta);
  t->data[2][0] = -sin(theta);
  t->data[2][2] = cos(theta);
  multiply(t, m);
  free_matrix(t);
}
void rotate_x_axis(MATRIX *m, double theta){
  MATRIX *t = generate_matrix(4, 4);
  ident(t);
  t->data[1][1] = cos(theta);
  t->data[1][2] = -sin(theta);
  t->data[2][1] = sin(theta);
  t->data[2][2] = cos(theta);
  multiply(t, m);
  free_matrix(t);
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
		     points[p1][0], points[p1][1], points[p1][2],	\
		     points[p2][0], points[p2][1], points[p2][2],	\
		     points[p3][0], points[p3][1], points[p3][2]);	\
	add_triangle(e,							\
		     points[p3][0], points[p3][1], points[p3][2],	\
		     points[p4][0], points[p4][1], points[p4][2],	\
		     points[p1][0], points[p1][1], points[p1][2]);})

  t(0, 1, 2, 3);
  t(0, 3, 7, 4);
  t(3, 2, 6, 7);
  t(2, 1, 5, 6);
  t(1, 0, 4, 5);
  t(7, 6, 5, 4);
  #undef t
}
ELEMENT * generate_sphere(double x, double y, double z, double r, int res){
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
  int res = 40;
  ELEMENT * s = generate_sphere(x, y, z, r, res);
  MATRIX * m = s->edge_matrix;
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
  free_element(s);
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
  free_element(s);
}

char *str_replace(char *orig, char *rep, char *with) {
  //https://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c
  char *result; // the return string
  char *ins;    // the next insert point
  char *tmp;    // varies
  int len_rep;  // length of rep (the string to remove)
  int len_with; // length of with (the string to replace rep with)
  int len_front; // distance between rep and end of last rep
  int count;    // number of replacements

  if (!orig || !rep)
    return NULL;
  len_rep = strlen(rep);
  if (len_rep == 0)
    return NULL; // empty rep causes infinite loop during count
  if (!with)
    with = "";
  len_with = strlen(with);

  ins = orig;
  for (count = 0; tmp = strstr(ins, rep); ++count) {
    ins = tmp + len_rep;
  }
  tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

  if (!result)
    return NULL;
  while (count--) {
    ins = strstr(orig, rep);
    len_front = ins - orig;
    tmp = strncpy(tmp, orig, len_front) + len_front;
    tmp = strcpy(tmp, with) + len_with;
    orig += len_front + len_rep; // move to next "end of rep"
  }
  strcpy(tmp, orig);
  return result;
}
