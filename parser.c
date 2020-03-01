#include "library.h"

void parse_file ( char * filename, MATRIX * transform, ELEMENT * e, GRID * s) {
  enum command{Line, Ident, Scale, Move, Rotate, Apply, Display, Save};
  char * commands[] = {"line", "ident", "scale", "move", "rotate", "apply", "display", "save"};
  printf("hello!\n");
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

      }
    }else{
      printf("%s\n", commands[c]);
      if(c == Ident || c == Apply || c == Display){
	switch(c){
	  case Ident:
	    ident(transform);
	    break;
	  case Apply:
	    printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaah\n");
	    multiply(transform, e->matrix);
	    break;
	  case Display:
	    break;
	}

	
      }else if(c == Save){
	line[strlen(line)-1]='\0';
	printf("save! %s \n", line);
	//write_image
      }else{
	line[strlen(line)-1]='\0';
	printf("d %s!\n", line);
	char *a[7] = {0, 0, 0, 0, 0, 0, 0};
	printf("here1?\n");	
	char *ptr = strtok(line, " ");
	int k = 0;
	while (ptr != NULL){
	  a[k++] = ptr;
	  ptr = strtok (NULL, " ");
	}
	printf("here2?\n");
	switch(c){
	  case Line:
	    printf("fish!\n");
	    add_line(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]),  atoi(a[4]),  atoi(a[5]));
	    break;
	  case Scale:
	    printf("%d %d %d --------------scaling!\n", atoi(a[0]), atoi(a[1]), atoi(a[2]));
	    scale(transform, atoi(a[0]), atoi(a[1]), atoi(a[2]));
	    printf("?????\n");
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

int main(){
  MATRIX * t = generate_matrix(4, 4);
  ident(t);
  ELEMENT * e = generate_element(40, rgb(255, 255, 255));
  GRID * g = generate_grid(400, 400);
  parse_file("script", t, e, g);
  return 0;
}
