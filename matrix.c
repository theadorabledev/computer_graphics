#include "library.h"
int main(){
  MATRIX * a = generate_matrix(4, 4);
  ident(a);
  MATRIX * b = generate_matrix(4, 5);
  for(int r = 0; r < b->rows; r++)
    for(int c = 0; c < b->columns; c++)
      b->data[r][c] = r * 10 + c;
  printf("Matrix A:\n");
  print_matrix(a);
  printf("Matrix B:\n");
  print_matrix(b);
  printf("A * B:\n");
  multiply(a, b);
  print_matrix(b);
  
  return 0;
}

