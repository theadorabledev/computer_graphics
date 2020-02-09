all: picmaker line
picmaker: picmaker.c library
	gcc -o picmaker picmaker.c library -lm
line: line.c library
	gcc -o line line.c library -lm
run:
	./picmaker
display:
	display *.png
convert:
	convert *.ppm -set filename:base "%[basename]" "%[filename:base].png"
library: library.c library.h
	gcc -o library -c library.c
clean:
	rm -f *~
	rm -f *.o
	rm -f *.ppm
