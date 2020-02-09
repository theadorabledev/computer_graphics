all: picmaker line
picmaker: picmaker.c library
	gcc -o picmaker picmaker.c library -lm
line: line.c library
	gcc -o line line.c library -lm
run:
	./picmaker
	./line
	make convert
	make clean
display:
	display *.png
convert:
	echo "Converting all ppms to pngs."
	convert *.ppm -set filename:base "%[basename]" "%[filename:base].png"
library: library.c library.h
	gcc -o library -c library.c
clean:
	echo "Cleaning all .o files and ppms."
	rm -f *~
	rm -f *.o
	rm -f *.ppm
