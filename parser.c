#include "library.h"

void parse_file ( char * filename, MATRIX * transform, ELEMENT * e, GRID * s) {
  enum command{Line, Ident, Scale, Move, Rotate, Apply, Display, Save};
  char * commands[] = {"line", "ident", "scale", "move", "rotate", "apply", "display", "save"};
  FILE *f;
  char line[256];
  clear_grid(s);
  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  enum command c = -1;
  while(fgets(line, 255, f) != NULL) {
    if(c == -1){
      line[strlen(line)-1]='\0';
      for(int k = 0; k < 8; k++)
	if(!strcmp(line, commands[k]))
	  c = k;
      if(c == Ident || c == Apply || c == Display){
	switch(c){
	  case Ident:
	    ident(transform);
	    break;
	  case Apply:
	    multiply(transform, e->matrix);
	    break;
	  case Display:
	    clear_grid(s);
	    plot_element(e, s);
	    write_image(s, "temp.ppm");
	    system("display temp.ppm");
	    break;
	}
	c = -1;
      }
    }else{
      if(c == Save){
	system("rm -rf temp.ppm");
	line[strlen(line)-1]='\0';
	write_image(s, line);
      }else{
	line[strlen(line)-1]='\0';
	char *a[7] = {0, 0, 0, 0, 0, 0, 0};
	char *ptr = strtok(line, " ");
	int k = 0;
	while (ptr != NULL){
	  a[k++] = ptr;
	  ptr = strtok (NULL, " ");
	}
	switch(c){
	  case Line:
	    add_line(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]),  atoi(a[4]),  atoi(a[5]));
	    break;
	  case Scale:
	    scale(transform, atoi(a[0]), atoi(a[1]), atoi(a[2]));
	    break;
	  case Move:
	    translate(transform, atoi(a[0]), atoi(a[1]), atoi(a[2]));
	    break;
	  case Rotate:
	    switch(a[0][0]){
	      case 'x': rotate_x_axis(transform, atoi(a[1]) * M_PI / 180.0);
		break;
	      case 'y': rotate_y_axis(transform, atoi(a[1]) * M_PI / 180.0);
		break;
	      case 'z': rotate_z_axis(transform, atoi(a[1]) * M_PI / 180.0);
		break;		
	    }
	    break;
	}
      }
      c = -1;
    }
  }
}

int main(int argc, char *argv[]){
  MATRIX * t = generate_matrix(4, 4);
  ident(t);
  ELEMENT * e = generate_element(40, 0);
  GRID * g = generate_grid(400, 400);
  printf("%s\n", argv[0]);
  parse_file(argv[1], t, e, g);
  return 0;
}
