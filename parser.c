#include "library.h"
#include "map/src/map.h"
#define BUILD0(x) atoi(x[0])
#define BUILD1(x) BUILD0(x), atoi(x[1])
#define BUILD2(x) BUILD1(x), atoi(x[2])
#define BUILD3(x) BUILD2(x), atoi(x[3])
#define BUILD4(x) BUILD3(x), atoi(x[4])
#define BUILD5(x) BUILD4(x), atoi(x[5])
#define BUILD6(x) BUILD5(x), atoi(x[6])
#define BUILD7(x) BUILD6(x), atoi(x[7])
#define BUILD8(x) BUILD7(x), atoi(x[8])
#define BUILD9(x) BUILD8(x), atoi(x[9])
#define BUILD10(x) BUILD9(x), atoi(x[10])
#define BUILD11(x) BUILD10(x), atoi(x[11])
#define BUILD(x, i) BUILD##i(x)

typedef struct loop LOOP;
typedef struct loop{
  int pos;
  int repeats;
  LOOP * next;
} LOOP;
void add_to_loop(LOOP ** loop, int pos, int repeats){
  LOOP * l = malloc(sizeof(LOOP));
  l->pos = pos;
  l->repeats = repeats - 1;
  l->next = *loop;
  *loop = l;
}
void pop_loop(LOOP ** loop){
  LOOP * l = (*loop)->next;
  free(*loop);
  *loop = l;
}
void trimleading(char *s){
  int i,j;
  for(i=0; s[i]==' '||s[i]=='\t';i++);

  for(j=0;s[i];i++){
    s[j++]=s[i];
  }
  s[j]='\0';
}

void parse_file ( char * filename, MATRIX * stack, ELEMENT * e, GRID * s) {
  MATRIX * transform = generate_matrix(4, 4);
  ident(transform);
  enum command{Comment, Display, Push, Pop, Loop_End, Loop, Set, Srand, Color, Line, Circle, Bezier, Hermite, Speckle, Flower, Tendril, Box, Sphere, Torus, Cone,  Scale, Move, Rotate, Save};
  char * commands[] = {"comment", "display", "push", "pop", "loop_end", "loop", "set", "srand", "color", "line", "circle", "bezier", "hermite", "speckle", "flower", "tendril", "box", "sphere", "torus", "cone", "scale", "move", "rotate", "save"};
  FILE *f;
  char line[256];
  LOOP * loop_stack;
  clear_grid(s);
  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");
  enum command c = -1;

  map_str_t m;
  map_init(&m);

  while(fgets(line, 255, f) != NULL) {
    trimleading(line);
    line[strlen(line)-1]='\0';
    if(!strlen(line))
      continue;
    char *a[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char *ptr = strtok(line, " ");
    int j = 0;
    int one_line = 0;
    while (ptr != NULL){
      a[j++] = ptr;
      ptr = strtok (NULL, " ");
    }
    for(int k = 0; k < 22; k++)
      if(!strcmp(a[0], commands[k]) || (k == 0 && a[0] && a[0][0] == '#'))
	c = k;
    if(c < 5){
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
	case Loop_End:
	  if(!(loop_stack->repeats)){
	    pop_loop(&loop_stack);
	  }else{
	    loop_stack->repeats -= 1;
	    fseek(f, loop_stack->pos, SEEK_SET);
	  }
	  break;
      }
      c = -1;
    }
    else{
      if(a[1]){
	one_line = 1;
      }else{
	fgets(line, 255, f);
	trimleading(line);
	line[strlen(line)-1]='\0';

	while(!strlen(line)){
	  fgets(line, 255, f);
	  trimleading(line);
	  line[strlen(line)-1]='\0';
	}

	ptr = strtok(line, " ");
	j = 0;
	while (ptr != NULL){
	  a[j++] = ptr;
	  ptr = strtok (NULL, " ");
	}
      }
      for(int i = 0; i < 12; i++){
	if(a[i] && a[i][0] == '$'){
	  char **t = map_get(&m, a[i] + 1);
	  a[i] = t ? *t : "0";
	}
      }
      switch(c){
	case Save:
	  system("rm -rf temp.ppm");
	  write_image(s, (a + one_line)[0]);
	  break;
	case Loop:
	  add_to_loop(&loop_stack, ftell(f), atoi((a + one_line)[0]));
	  break;
	case Set:{
	  map_set(&m, (a + one_line)[0], strcpy(malloc(strlen((a + one_line)[1])), (a + one_line)[1]));
	  break;
	}
	case Srand:
	  srand(atoi((a + one_line)[0]));
	  break;
	case Color:
	  if(atoi((a + one_line)[0]) < 0)
	    set_color(e, atoi((a + one_line)[0]));
	  else
	    set_color(e, rgb(BUILD((a + one_line), 2)));
	  break;
	case Line:
	  add_line(e, BUILD((a + one_line), 5));
	  break;
	case Circle:
	  circle(e, BUILD((a + one_line), 3));
	  break;
	case Bezier:{
	  int positions[] = {atoi((a + one_line)[0]), atoi((a + one_line)[1]), 0,
			     atoi((a + one_line)[2]), atoi((a + one_line)[3]), 0,
			     atoi((a + one_line)[4]), atoi((a + one_line)[5]), 0,
			     atoi((a + one_line)[6]), atoi((a + one_line)[7]), 0};
	  bezier(e, positions, 3, .05);
	  break;
	}

	case Hermite:{
	  double data[] = {atoi((a + one_line)[0]), atoi((a + one_line)[1]), 0,
			   atoi((a + one_line)[2]), atoi((a + one_line)[3]), 0,
			   atoi((a + one_line)[4]), atoi((a + one_line)[5]), 0,
			   atoi((a + one_line)[6]), atoi((a + one_line)[7]), 0};
	  hermite(e, data, .05);
	  break;
	}
	case Speckle:
	  speckle(e, BUILD((a + one_line), 8));
	  break;
	case Flower:
	  flower(e, BUILD((a + one_line), 8));
	  break;
	case Tendril:
	  tendril(e, BUILD((a + one_line), 9));
	  break;
	case Box:
	  box(e, BUILD((a + one_line), 5));
	  break;
	case Sphere:
	  sphere(e, BUILD((a + one_line), 3));
	  break;
	case Torus:
	  torus(e, BUILD((a + one_line), 4));
	  break;
	case Cone:
	  cone(e, atoi((a + one_line)[0]),  atoi((a + one_line)[1]),  atoi((a + one_line)[2]),
	       degrees_to_radians(atoi((a + one_line)[3])),  degrees_to_radians(atoi((a + one_line)[4])),  degrees_to_radians(atoi((a + one_line)[5])),
	       atoi((a + one_line)[6]), atoi((a + one_line)[7]), atoi((a + one_line)[8]));
	  break;
	case Scale:
	  scale(transform, BUILD((a + one_line), 2));
	  transform_stack(stack, transform);
	  ident(transform);
	  break;
	case Move:
	  translate(transform, BUILD((a + one_line), 2));
	  transform_stack(stack, transform);
	  ident(transform);
	  break;
	case Rotate:
	  switch((a + one_line)[0][0]){
	    case 'x': rotate_x_axis(transform, atoi((a + one_line)[1]) * M_PI / 180.0);
	      break;
	    case 'y': rotate_y_axis(transform, atoi((a + one_line)[1]) * M_PI / 180.0);
	      break;
	    case 'z': rotate_z_axis(transform, atoi((a + one_line)[1]) * M_PI / 180.0);
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
      c = -1;
    }

  }
}

int main(int argc, char *argv[]){
  srand(time(0));
  MATRIX * t = generate_matrix(4, 4);
  ident(t);
  ELEMENT * e = generate_element(40, 0);
  GRID * g = generate_grid(500, 500);
  parse_file(argv[1], t, e, g);
  return 0;
}
