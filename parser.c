#include "library.h"
#include "map/src/map.h"
#define BUILD0(x) atof(x[0])
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

#define MAX_ARGS 256
enum command{Comment, Display, Clear, Push, Pop, End_For, End_If, End_Function, Function, Else, If, For, Set, Srand, Color, Light, Texture,  Line, Circle, Bezier, Hermite, Speckle, Flower, Tendril, Box, Sphere, Torus, Cone,  Scale, Move, Rotate, Save, Gif, Function_Call};
typedef struct command_struct COMMAND;
typedef struct command_struct{
  int pos;
  COMMAND * next_canon;//Next in terms of line placement in the file
  COMMAND * next_true;//Next command to execute if this is true
  COMMAND * next_false;//Next command to execute if this is false
  COMMAND * parent;
  map_str_t variables;
  enum command  type;
  char * args;
} COMMAND;
typedef map_t(COMMAND *) function_map;
typedef struct parse_data{
  COMMAND * starting_command;
  function_map f_map;
}PARSE_DATA;
void map_clean(map_str_t m){
  char *key;
  map_iter_t iter = map_iter(&m);
  while (key = map_next(&m, &iter)){
    if(key){
      char * res = *map_get(&m, key);
      free(res);
    }
  }
  map_deinit(&m);
}
void print_map(map_str_t m){
  char *key;
  map_iter_t iter = map_iter(&m);
  while (key = map_next(&m, &iter)){
    char * res = *map_get(&m, key);
    printf("    (%-20s) --> (%s)\n", key, res);
  }
}
void print_variables(COMMAND * c){
  if(!c)
    return;
  print_variables(c->parent);
  printf("========|(%-3d) %-50.50s|========\n", c->pos, c->args);
  print_map(c->variables);
}

typedef struct stack STACK;
typedef struct stack {
  STACK * next;
  COMMAND * command;
  COMMAND * command_true_end;
  COMMAND * command_false_end;
}STACK;



void trimleading(char *s){
  int i,j;
  for(i=0; s[i]==' '||s[i]=='\t';i++);

  for(j=0;s[i];i++){
    s[j++]=s[i];
  }
  s[j]='\0';
}
char ** get_variable_value(COMMAND * command, char * var){
  if(map_get(&command->variables, var))
    return map_get(&command->variables, var);
  if(command->parent)
    return get_variable_value(command->parent, var);
  return NULL;
}
char ** eval(COMMAND * command, char * string){
  if(string[0] == '#')
    return 0;
  char * new_string = calloc(1024 * sizeof(char), 1);
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
      char ** real_res = eval(command, sub);
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
  //Tokenize the string and substitute variables with their values
  char **args = calloc(MAX_ARGS * sizeof(char *), 1);
  char *ptr = strtok(new_string, " ");
  int j = 0;
  while (ptr != NULL){
    args[j++] = STR_COPY(ptr);
    ptr = strtok(NULL, " ");
  }
  for(int i = 0; i < MAX_ARGS && args[i]; i++){
    if(args[i] && args[i][0] == '$'){
      char **t = get_variable_value(command, args[i] + 1);
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
      }else if(args[i + 1] && !strcmp(args[i + 1], "&&")){
	sprintf(buf, "%d", atof(args[i]) && atof(args[i + 2]));
      }else if(args[i + 1] && !strcmp(args[i + 1], "||")){
	sprintf(buf, "%d", atof(args[i]) || atof(args[i + 2]));
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
STACK * stack_add(STACK * s, COMMAND * c){
  STACK * new = calloc(sizeof(STACK), 1);
  new->command = c;
  new->next = s;
  return new;
}
STACK * stack_pop(STACK * s){
  STACK * new = s->next;
  free(s);
  return new;
}
COMMAND * generate_command(char * string, int type, COMMAND * parent){
  COMMAND *c = calloc(sizeof(COMMAND), 1);
  c->next_canon = NULL;
  c->next_true = NULL;
  c->next_false = NULL;
  c->args = STR_COPY(string);
  c->type = type;
  c->parent = parent;
  switch(type){
    case Function:
    case For:
      map_init(&c->variables);
  }
  return c;
}
void free_command(COMMAND * command){
  free(command->args);
  switch(command->type){
    case Function:
    case For:
      map_clean(command->variables);
  }
  free(command);
}
COMMAND * parse_file(function_map * f_map, char * filename){
  char * commands[] = {"comment", "display", "clear", "push", "pop", "end_for", "end_if", "end_function", "function", "else", "if", "for", "set", "srand", "color", "light", "texture", "line", "circle", "bezier", "hermite", "speckle", "flower", "tendril", "box", "sphere", "torus", "cone", "scale", "move", "rotate", "save", "gif"};
  FILE *f;
  char line[256];
  bzero(line, 256);
  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");
  int pos = 0;
  COMMAND * last = NULL;
  COMMAND * main = NULL;
  STACK * stack = NULL;
  int first = 1;
  while(fgets(line, 255, f) != NULL){
    trimleading(line);
    line[strlen(line)-1]='\0';
    if(!strlen(line))
      continue;
    if(line[0] == '#')
      continue;
    int built_in = 0;
    for(int c = 0; c < 33; c++){
      if(!strncmp(line, commands[c], strlen(commands[c]))){
	COMMAND * command = generate_command(line, c, stack ? stack->command : NULL);
	if(last){
	  last->next_canon = command;
	}
	command->pos = pos++;
	switch(c){
	  case Function:
	    stack = stack_add(stack, command);
	    break;
	  case If:
	  case For:
	    stack = stack_add(stack, command);
	    last->next_true = command;
	    break;
	  case Else:
	    stack->command->next_false = command;
	    stack->command_true_end = last;
	    break;
	  case End_If:
	    last->next_true = command;
	    if(stack->command_true_end)
	      stack->command_true_end->next_true = command;
	    stack = stack_pop(stack);
	    break;
	  case End_For:
	    stack->command->next_false = command;
	    last->next_true = stack->command;
	    stack = stack_pop(stack);
	    break;
	  case End_Function:
	    last->next_true = command;
	    stack->command->next_false = command;
	    char ** args = eval(NULL, stack->command->args);
	    map_set(f_map, args[1], stack->command);
	    for(int i = 0; i < MAX_ARGS; i++)
	      if(args[i])
		free(args[i]);
	    free(args);
	    stack = stack_pop(stack);
	    break;
	  default:
	    if(last)
	      last->next_true = command;
	}
	if(first)
	  main = command;
	first = 0;
	last = command;
	built_in = 1;
	break;
      }
    }
    if(!built_in){
      char str[256];
      strncpy(str, line, 255);
      char * name = strtok(str, " ");
      COMMAND * res = *map_get(f_map, name);
      COMMAND * command = generate_command(line, Function_Call, stack ? stack->command : NULL);
      last->next_canon = command;
      command->pos = pos++;
      last->next_true = command;
      command->next_false = res;
      last = command;
    }
    bzero(line, 256);
  }
  fclose(f);
  COMMAND * n = main;
  while(n){
    printf("(%-3d) %-12.12s |", n->pos, n->args);
    printf("(%-3d) %-12.12s |",
	   n->next_true ? n->next_true->pos : -1,
	   n->next_true ? n->next_true->args : "");
    printf("(%-3d) %-12.12s |",
	   n->next_false ? n->next_false->pos : -1,
	   n->next_false ? n->next_false->args : "");
    printf("\n");
    n = n->next_canon;
  }
  return main;
}

void execute_commands (COMMAND * func){
  MATRIX * stack = generate_matrix(4, 4);
  ident(stack);
  ELEMENT * e = generate_element(40, 0);
  GRID * s = generate_grid(500, 500);
  MATRIX * transform = generate_matrix(4, 4);
  ident(transform);
  while(func){
    char **args = eval(func, func->args);
    if(func->type > 6){
	args++;
    }
    int end_true = 1;
    printf("(%-3d) |", func->pos);
    for(int i = 0 - (func->type > 6); i < MAX_ARGS && args[i]; i++)
      printf("%-12.12s |", args[i]);
    printf("\n");
    switch(func->type){
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
	system(buf);
	sprintf(buf, "animate %s", args[1]);
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
	char **t = get_variable_value(func, args[0]);
	int t_set = 0;
	if(!t){
	  map_set(&func->variables, args[0], STR_COPY(args[1]));
	  t_set = 1;
	  t = get_variable_value(func, args[0]);
	}
	if(!t_set){
	  char new[100];
	  bzero(new, 100);
	  sprintf(new, "%f", atof(*t) + atof(args[3]));
	  free(*t);
	  *t = STR_COPY(new);
	}
	if(atof(*t) >= atof(args[2])){
	  end_true = 0;
	  map_clean(func->variables);
	  map_init(&func->variables);
	}
	break;
      case Function_Call:
	end_true = 0;
	func->next_false->next_false->next_false = func->next_true;
	map_init(&func->next_false->variables);
	char ** func_args = eval(func->next_false, func->next_false->args);
	for(int i = 2; i < MAX_ARGS && func_args[i]; i++){
	  map_set(&func->next_false->variables, func_args[i], STR_COPY(args[i - 2]));
	}
	for(int i = 0; i < MAX_ARGS; i++)
	  if(func_args[i])
	    free(func_args[i]);
	free(func_args);
	break;
      case Function:{
	//print_variables(func);
	char * key;
	map_iter_t iter = map_iter(&m);
	while (key = map_next(&func->variables, &iter)){
	  if(key){
	    char * res = *map_get(&func->variables, key);
	  }
	}
	break;
      }
      case End_Function:
	end_true = 0;
	map_clean(func->parent->variables);
	break;
      case If:
	map_init(&func->variables);
	if(args[0][0] == '0')
	  end_true = 0;
	break;
      case End_If:
	map_clean(func->parent->variables);
	break;
      case Set:{
	char **t = get_variable_value(func, args[0]);
	if(t){
	  free(*t);
	  *t = STR_COPY(args[1]);
	}else{
	  map_set(&func->parent->variables, args[0], STR_COPY(args[1]));
	}
	break;
      }
      case Srand:
	if(!args[0])
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
    if(func->type > 6)
      args--;
    for(int i = 0; i < MAX_ARGS; i++)
      if(args[i])
	free(args[i]);
    free(args);
    if(end_true)
      func = func->next_true;
    else
      func = func->next_false;
  }
  while(stack)
    stack = pop_from_stack(stack);
  free_grid(s);
  free_matrix(transform);
  free_element(e);
}
int main(int argc, char *argv[]){
  srand(time(0));
  function_map f_map;
  map_init(&f_map);
  COMMAND * start = parse_file(&f_map, argv[1]);
  COMMAND * main = *map_get(&f_map, "main");
  execute_commands(main);
  while(start){
    COMMAND * next = start->next_canon;
    free(start->args);
    free(start);
    start = next;
  }
  map_deinit(&f_map);

  return 0;
}

