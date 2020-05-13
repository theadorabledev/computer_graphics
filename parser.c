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

#define MAX_ARGS 12
typedef struct loop LOOP;
typedef struct loop{
  int pos;
  char * var;
  int start;
  int end;
  int inc;
  LOOP * next;
} LOOP;
void add_to_loop(LOOP ** loop, int pos, char * var, int start, int end, int inc){
  LOOP * l = malloc(sizeof(LOOP));
  l->pos = pos;
  l->var = var;
  l->start = start;
  l->end = end - inc;
  l->inc = inc;
  l->next = *loop;
  *loop = l;
}
void pop_loop(LOOP ** loop){
  LOOP * l = (*loop)->next;
  free((*loop)->var);
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

char ** eval(map_str_t m, char * string){
  if(string[0] == '#')
    return 0;
  char * new_string = calloc(256 * sizeof(char), 1);
  memcpy(new_string, string, strlen(string));
  trimleading(new_string);

  //Recursively evaluate items in parentheses
  int p_start = 0;
  int p_end = strlen(new_string);
  int paren = 0;
  int i = 0;
  while(i < strlen(new_string)){
    if(new_string[i] == '(' && (paren++ == 0)){
      p_start = i + 1;
    }
    if(new_string[i] == ')' && (--paren == 0)){
      p_end = i;
      char *sub = calloc(sizeof(char) * (p_end - p_start + 2 + 1), 1);
      strncpy(sub, new_string + p_start, p_end - p_start);
      char ** real_res = eval(m, sub);
      char *res = STR_COPY(real_res[0]);
      strncpy(sub, new_string + p_start - 1, p_end - p_start + 2);
      char *new = str_replace(new_string, sub, res);
      free(new_string);
      free(sub);
      free(res);
      for(int i = 0; i < MAX_ARGS; i++)
	free(real_res[i]);
      free(real_res);
      new_string = new;
      i = p_start;
    }
    i++;
  }
  char **args = calloc(MAX_ARGS * sizeof(char *), 1);
  char *ptr = strtok(new_string, " ");
  int j = 0;
  //Tokenize the string and substitute variables with their values
  while (ptr != NULL){
    args[j++] = STR_COPY(ptr);
    ptr = strtok(NULL, " ");
  }
  for(int i = 0; i < MAX_ARGS; i++){
    if(args[i] && args[i][0] == '$'){
      char **t = map_get(&m, args[i] + 1);
      if(t){
	free(args[i]);
	args[i] = STR_COPY(*t);
      }
    }
  }
  //Perform the operations on the string
  char ** new_args = calloc((MAX_ARGS + 1) * sizeof(char *), 1);
  int p = 0;
  for(int i = 0; i < MAX_ARGS; i++){
    char buf[50];
    int mod = 2;
    int inc = 0;
    if(args[i]){
      if(args[i + 1] && !strcmp(args[i + 1], "+")){
	sprintf(buf, "%f", atof(args[i]) + atof(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], "-")){
	sprintf(buf, "%f", atof(args[i]) - atof(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], "*")){
	sprintf(buf, "%f", atof(args[i]) * atof(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], "/")){
	sprintf(buf, "%f", atof(args[i]) / atof(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], "%")){
	sprintf(buf, "%d", atoi(args[i]) % atoi(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], "<")){
	sprintf(buf, "%d", atoi(args[i]) < atoi(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], "<=")){
	sprintf(buf, "%d", atoi(args[i]) <= atoi(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], ">")){
	sprintf(buf, "%d", atoi(args[i]) > atoi(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], ">=")){
	sprintf(buf, "%d", atoi(args[i]) >= atoi(args[i + 2]));
      }else if(!strcmp(args[i], "rand")){
	sprintf(buf, "%d", rand() % atoi(args[i + 1]));
	mod = 1;
	inc = 1;
      }else if(!strcmp(args[i], "sin")){
	sprintf(buf, "%f", sin(degrees_to_radians(atoi(args[i + 1]))));
	mod = 1;
	inc = 1;
      }else if(!strcmp(args[i], "cos")){
	sprintf(buf, "%f", cos(degrees_to_radians(atoi(args[i + 1]))));
	mod = 1;
	inc = 1;
      }else{
	bzero(buf, 50);
	memcpy(buf, args[i], strlen(args[i]));
	mod = 0;
	inc = 0;
      }
      free(new_args[p]);
      new_args[p++] = STR_COPY(buf);
      if(args[i + mod])
	free(args[i + mod]);
      args[i + mod] = STR_COPY(buf);
      i += inc;
    }
  }
  free(new_string);
  for(int i = 0; i < MAX_ARGS; i++){
    if(args[i])
      free(args[i]);
  }
  free(args);
  return new_args;
}
void parse_file (char * filename){
  MATRIX * stack = generate_matrix(4, 4);
  ident(stack);
  ELEMENT * e = generate_element(40, 0);
  GRID * s = generate_grid(500, 500);
  MATRIX * transform = generate_matrix(4, 4);
  ident(transform);
  enum command{Comment, Display, Clear, Push, Pop, End_For, For, Set, Srand, Color, Light, Texture,  Line, Circle, Bezier, Hermite, Speckle, Flower, Tendril, Box, Sphere, Torus, Cone,  Scale, Move, Rotate, Save, Gif};
  char * commands[] = {"comment", "display", "clear", "push", "pop", "end_for", "for", "set", "srand", "color", "light", "texture", "line", "circle", "bezier", "hermite", "speckle", "flower", "tendril", "box", "sphere", "torus", "cone", "scale", "move", "rotate", "save", "gif"};
  FILE *f;
  char line[256];
  bzero(line, 256);
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
    char **args = eval(m, line);
    if(!args)
      continue;
    for(int k = 0; k < 28; k++)
      if(!strcmp(args[0], commands[k]) || (k == 0 && args[0] && args[0][0] == '#'))
	c = k;
    if(c > 5){
	args++;
    }
    /* for(int i = 0 - (c > 5); i < MAX_ARGS && args[i]; i++) */
    /*   printf("%s | ", args[i]); */
    /* printf("\n"); */
    switch(c){
      case Display:
	if(args[1]){
	  char buf[30];
	  sprintf(buf, "display %s", args[1]);
	  printf("%s\n", buf);
	  system(buf);
	}else{
	  write_image(s, "temp.ppm");
	  system("display temp.ppm");
	}
	break;
      case Clear:
	clear_grid(s);
      case Push:
	stack = push_to_stack(stack);
	break;
      case Pop:
	stack = pop_from_stack(stack);
	break;
      case End_For:{
	char ** s = map_get(&m, loop_stack->var);
	if(atoi(*s) >= loop_stack->end){
	  map_remove(&m, loop_stack->var);
	  pop_loop(&loop_stack);
	}else{
	  sprintf(*s, "%d", atoi(*s) + loop_stack->inc);
	  fseek(f, loop_stack->pos, SEEK_SET);
	}
	break;
      }
      case Save:{
	system("rm -rf temp.ppm");
	char buf[30];
	if(args[1])
	  sprintf(buf, "%03d_", (int) atoi(args[1]));
	strcat(buf, args[0]);
	write_image(s, buf);
	break;
      }
      case Gif:{
	char buf[100];
	sprintf(buf, "convert -delay 10 -loop 0 *_%s %s && rm *_%s", args[0], args[1], args[0]);
	printf("%s\n", buf);
	system(buf);
	break;
      }
      case For:
	//For var start stop inc
	//For var start stop -> var start stop 1
	//For var stop -> 0 stop 1
	if(!args[3]){
	  if(args[2]){
	    free(args[3]);
	    char * t = "1";
	    args[3] = t;
	  }else if(args[1]){
	    free(args[2]);
	    args[2] = STR_COPY(args[1]);
	    free(args[1]);
	    char * t = "0";
	    args[1] = STR_COPY(t);;
	    free(args[3]);
	    t = "1";
	    args[3] = STR_COPY(t);;
	  }
	}
	add_to_loop(&loop_stack, ftell(f), STR_COPY(args[0]), BUILD((args + 1), 2));
	char **t = map_get(&m, args[0]);
	if(t)
	  free(*t);
	map_set(&m, loop_stack->var, STR_COPY(args[1]));
	break;
      case Set:{
	char **t = map_get(&m, args[0]);
	if(t)
	  free(*t);
	map_set(&m, args[0], STR_COPY(args[1]));
	break;
      }
      case Srand:
	if(!atoi(args[0]))
	  srand(time(NULL));
	else
	  srand(atoi(args[0]));
	break;
      case Color:
	if(atoi(args[0]) < 0)
	  set_color(e, atoi(args[0]));
	else
	  set_color(e, rgb(BUILD(args, 2)));
	break;
      case Light:
	//Color defaults to white if no color specified
	if(args[3])
	  add_light(e, BUILD(args, 5));
	else
	  add_light(e, BUILD(args, 2), 255, 255, 255);
	break;
      case Texture:
	set_texture(e, atof(args[0]), atof(args[1]), atof(args[2]));
	break;
      case Line:
	add_line(e, BUILD(args, 5));
	break;
      case Circle:
	circle(e, BUILD(args, 3));
	break;
      case Bezier:{
	int positions[] = {atoi(args[0]), atoi(args[1]), 0,
			   atoi(args[2]), atoi(args[3]), 0,
			   atoi(args[4]), atoi(args[5]), 0,
			   atoi(args[6]), atoi(args[7]), 0};
	bezier(e, positions, 3, .05);
	break;
      }
      case Hermite:{
	double data[] = {atoi(args[0]), atoi(args[1]), 0,
			 atoi(args[2]), atoi(args[3]), 0,
			 atoi(args[4]), atoi(args[5]), 0,
			 atoi(args[6]), atoi(args[7]), 0};
	hermite(e, data, .05);
	break;
      }
      case Speckle:
	speckle(e, BUILD(args, 8));
	break;
      case Flower:
	flower(e, BUILD(args, 8));
	break;
      case Tendril:
	tendril(e, BUILD(args, 9));
	break;
      case Box:
	box(e, BUILD(args, 5));
	break;
      case Sphere:
	sphere(e, BUILD(args, 3));
	break;
      case Torus:
	torus(e, BUILD(args, 4));
	break;
      case Cone:
	cone(e, atoi(args[0]),  atoi(args[1]),  atoi(args[2]),
	     degrees_to_radians(atoi(args[3])),  degrees_to_radians(atoi(args[4])),  degrees_to_radians(atoi(args[5])),
	     atoi(args[6]), atoi(args[7]), atoi(args[8]));
	break;
      case Scale:
	scale(transform, BUILD(args, 2));
	transform_stack(stack, transform);
	ident(transform);
	break;
      case Move:
	translate(transform, BUILD(args, 2));
	transform_stack(stack, transform);
	ident(transform);
	break;
      case Rotate:
	switch(args[0][0]){
	  case 'x': rotate_x_axis(transform, atoi(args[1]) * M_PI / 180.0);
	    break;
	  case 'y': rotate_y_axis(transform, atoi(args[1]) * M_PI / 180.0);
	    break;
	  case 'z': rotate_z_axis(transform, atoi(args[1]) * M_PI / 180.0);
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
    if(c > 5)
      args--;
    for(int i = 0; i < MAX_ARGS; i++)
      if(args[i])
	free(args[i]);
    free(args);
    bzero(line, 256);
    c = -1;
  }
  while(stack){
    stack = pop_from_stack(stack);
  }
  free_grid(s);
  free_matrix(transform);
  free_element(e);
  fclose(f);
  map_deinit(&m);
}

int main(int argc, char *argv[]){
  srand(time(0));

  parse_file(argv[1]);
  return 0;
}
