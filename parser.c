#include "library.h"

void parse_file ( char * filename, MATRIX * stack, ELEMENT * e, GRID * s) {
  MATRIX * transform = generate_matrix(4, 4);
  ident(transform);
  enum command{Comment, Display, Push, Pop, Color, Line, Circle, Bezier, Hermite, Speckle, Flower, Tendril, Box, Sphere, Torus, Cone,  Scale, Move, Rotate, Save};
  char * commands[] = {"comment", "display", "push", "pop", "color", "line", "circle", "bezier", "hermite", "speckle", "flower", "tendril", "box", "sphere", "torus", "cone", "scale", "move", "rotate", "save"};
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
      for(int k = 0; k < 19; k++)
	if(!strcmp(line, commands[k]) || (k == 0 && line[0] == '#'))
	  c = k;
      if(c < 4){
	switch(c){
	  case Display:
	    write_image(s, "temp.ppm");
	    system("display temp.ppm");
	    break;
	  case Push:
	    stack = push_to_stack(stack);
	    break;
	  case Pop:
	    stack = pop_from_stack(stack);
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
	char *a[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	char *ptr = strtok(line, " ");
	int k = 0;
	while (ptr != NULL){
	  a[k++] = ptr;
	  ptr = strtok (NULL, " ");
	}
	switch(c){
	  case Color:
	    if(atoi(a[0]) < 0)
	      set_color(e, atoi(a[0]));
	    else
	      set_color(e, rgb(atoi(a[0]), atoi(a[1]), atoi(a[2])));
	    break;
	  case Line:
	    add_line(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]),  atoi(a[4]),  atoi(a[5]));
	    break;
	  case Circle:
	    circle(e, atoi(a[0]), atoi(a[1]), atoi(a[2]), atoi(a[3]));
	    break;
	  case Bezier:{
	    int positions[] = {atoi(a[0]), atoi(a[1]), 0,
			       atoi(a[2]), atoi(a[3]), 0,
			       atoi(a[4]), atoi(a[5]), 0,
			       atoi(a[6]), atoi(a[7]), 0};
	    bezier(e, positions, 3, .05);
	    break;
	  }
	  
	  case Hermite:{
	    double data[] = {atoi(a[0]), atoi(a[1]), 0,
			     atoi(a[2]), atoi(a[3]), 0,
			     atoi(a[4]), atoi(a[5]), 0,
			     atoi(a[6]), atoi(a[7]), 0};
	    hermite(e, data, .05);
	    break;
	  }
	  case Speckle:
	    speckle(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]),  atoi(a[4]),  atoi(a[5]), atoi(a[6]), atoi(a[7]), atoi(a[8]));
	    break;
	  case Flower:
	    flower(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]),  atoi(a[4]),  atoi(a[5]), atoi(a[6]), atoi(a[7]), atoi(a[8]));
	    break;
	  case Tendril:
	    //void tendril(ELEMENT * e, int x, int y, int z, int theta, int phi, int variance, int length, int radius){
	    tendril(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]),  atoi(a[4]),  atoi(a[5]), atoi(a[6]), atoi(a[7]), atoi(a[8]));
	    break;
	  case Box:
	    box(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]),  atoi(a[4]),  atoi(a[5]));
	    break;
	  case Sphere:
	    sphere(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]));
	    break;
	  case Torus:
	    torus(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  atoi(a[3]), atoi(a[4]));	    
	    break;
	  case Cone:
	    cone(e, atoi(a[0]),  atoi(a[1]),  atoi(a[2]),  degrees_to_radians(atoi(a[3])),  degrees_to_radians(atoi(a[4])),  degrees_to_radians(atoi(a[5])), atoi(a[6]), atoi(a[7]), atoi(a[8]));
	    break;
	  case Scale:
	    scale(transform, atoi(a[0]), atoi(a[1]), atoi(a[2]));
	    transform_stack(stack, transform);
	    ident(transform);
	    break;
	  case Move:
	    translate(transform, atoi(a[0]), atoi(a[1]), atoi(a[2]));
	    transform_stack(stack, transform);
	    ident(transform);
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
	    transform_stack(stack, transform);
	    ident(transform);
	    break;
	}
	multiply(stack, e->edge_matrix);
	multiply(stack, e->triangle_matrix);
	plot_element(e, s);
	clear(e);
      }
      c = -1;
    }
  }
}

int main(int argc, char *argv[]){
  srand(20);
  MATRIX * t = generate_matrix(4, 4);
  ident(t);
  ELEMENT * e = generate_element(40, 0);
  GRID * g = generate_grid(500, 500);
  parse_file(argv[1], t, e, g);
  return 0;
}
