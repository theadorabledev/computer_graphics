all: picmaker line matrix parser run

picmaker: picmaker.c library
	gcc -o picmaker picmaker.c library -lm
line: line.c library
	gcc -o line line.c library -lm
matrix: matrix.c library
	gcc -o matrix matrix.c library -lm
parser: parser.c library
	gcc -o parser parser.c library -lm
run: picmaker line matrix parser
	#./picmaker
	#./line
	#./matrix
	./parser
	#make convert
	#make clean
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
