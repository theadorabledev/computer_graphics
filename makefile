CC = gcc

RR = $(MAKECMDGOALS)
PV = ./parser
#PV = ./mdl_parser
RC = $(PV)
BINARIES = map_bin library mdl_parser
ifeq ($(MAKECMDGOALS), debug)
	CC = gcc -g
	RC = gdb -ex "set print pretty on" --args $(PV)
else ifeq ($(MAKECMDGOALS), memcheck)
	CC = gcc -g
	RC = valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all $(PV)
endif

all: $(BINARIES) run
memcheck: clobber $(BINARIES) run
debug: clobber $(BINARIES) run
map_bin: map/src/map.c map/src/map.h
	$(CC) -o map_bin -c map/src/map.c
picmaker: picmaker.c library
	$(CC) -o picmaker picmaker.c library -lm
line: line.c library
	$(CC) -o line line.c library -lm
matrix: matrix.c library
	$(CC) -o matrix matrix.c library -lm
bezier: bezier.c library
	$(CC) -o bezier bezier.c library -lm

mdl_parser: lex.yy.c y.tab.c y.tab.h symtab.o
	$(CC) -o mdl_parser lex.yy.c y.tab.c symtab.o library -lm

lex.yy.c: mdl/mdl.l y.tab.h
	flex -I mdl/mdl.l

y.tab.c: mdl/mdl.y mdl/symtab.h mdl/mdl_parser.h
	bison -d -y mdl/mdl.y

y.tab.h: mdl/mdl.y
	bison -d -y mdl/mdl.y

symtab.o: mdl/symtab.c mdl/mdl_parser.h
	$(CC) -c mdl/symtab.c



parser: parser.c map library
	$(CC) -o parser parser.c map_bin library -lm
run: picmaker line matrix parser bezier
	#$(RC) simple_anim.mdl
	#$(RC) eval_test
	$(RC) our_lord
	#./parser tendril_script
	#make convert
	make clean

display:
	display *.png
convert:
	@echo "Converting all ppms to pngs."
	convert *.ppm -set filename:base "%[basename]" "%[filename:base].png"
library: library.c library.h
	$(CC) -o library -c library.c
clean:
	@echo "Cleaning all .o files and ppms."
	rm -f *~
	rm -f *.o
	rm -f *.ppm
	@echo "Cleaning parser files."
	rm -f y.tab.c y.tab.h
	rm -f lex.yy.c
	rm -rf mdl_parser.dSYM
clobber:
	@echo "Removing all binaries."
	rm -f $(BINARIES)
