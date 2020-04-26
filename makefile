CC = gcc
RC = ./parser
RR = $(MAKECMDGOALS)
BINARIES = map_bin library parser
ifeq ($(MAKECMDGOALS), debug)
	CC = gcc -g
	RC = gdb --args ./parser
else ifeq ($(MAKECMDGOALS), memcheck)
	CC = gcc -g
	RC = valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all ./parser
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
parser: parser.c map library
	$(CC) -o parser parser.c map_bin library -lm
run: picmaker line matrix parser bezier
	$(RC) script
	#./parser tendril_script
	#make convert
	make clean

display:
	display *.png
convert:
	echo "Converting all ppms to pngs."
	convert *.ppm -set filename:base "%[basename]" "%[filename:base].png"
library: library.c library.h
	$(CC) -o library -c library.c
clean:
	@echo "Cleaning all .o files and ppms."
	rm -f *~
	rm -f *.o
	rm -f *.ppm
clobber:
	@echo "Removing all binaries."
	rm $(BINARIES)
