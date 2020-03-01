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
  double slope = (x2 - x1) ? (y2 - y1) * 1.0 / (x2 - x1) : 0;
  if(fabs(slope) <= 1.0){
    draw_line_gentle(grid, x1, y1, x2, y2, rgb);
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
void add_line(ELEMENT * e, int x, int y, int z, int x2, int y2, int z2){
  double ** m = e->matrix->data;
  m[0][e->length] = x;
  m[1][e->length] = y;
  m[2][e->length] = z;
  m[3][e->length] = 1;
  m[0][e->length + 1] = x2;
  m[1][e->length + 1] = y2;
  m[2][e->length + 1] = z2;
  m[3][e->length + 1] = 1;
  e->length += 2;
  if(e->length == e->matrix->columns)
    grow_matrix(e->matrix, (e->matrix->columns * 3)/2);
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
void free_matrix(MATRIX * m){
  for (int i = 0; i < m->rows; i++) {
    free(m->data[i]);
  }
  free(m->data);
  free(m);
}
void print_matrix(MATRIX * m){
  for(int r = 0; r < m->rows; r++){
    for(int c = 0; c < m->columns; c++){
      printf("%-7.2f ", m->data[r][c]);
    }
    printf("\n");
  }
  printf("\n");
}
void grow_matrix(MATRIX *m, int newcols){
  for (int i = 0; i < m->rows; i++) {
      m->data[i] = realloc(m->data[i],newcols*sizeof(double));
  }
  m->columns = newcols;
}
void copy_matrix(MATRIX * a, MATRIX * b){
  for (int r=0; r < a->rows; r++) 
    for (int c=0; c < a->columns; c++)  
      b->data[r][c] = a->data[r][c];  
}
void multiply(MATRIX * a, MATRIX * b){
  MATRIX * t = generate_matrix(b->rows, b->columns);
  for (int r=0; r < a->rows; r++){
    for (int c=0; c < b->columns; c++){
      for(int i = 0; i < a->rows; i++) {
	t->data[r][c] += a->data[r][i] * b->data[i][c];
      }
    }
  }
  copy_matrix(t, b);
  free_matrix(t);
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
