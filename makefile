all: picmaker line matrix parser bezier run

picmaker: picmaker.c library
	gcc -o picmaker picmaker.c library -lm
line: line.c library
	gcc -o line line.c library -lm
matrix: matrix.c library
	gcc -o matrix matrix.c library -lm
parser: parser.c library
	gcc -o parser parser.c library -lm
bezier: bezier.c library
	gcc -o bezier bezier.c library -lm
run: picmaker line matrix parser bezier
	#./picmaker
	#./line
	#./matrix
	#echo "Testing parser functionality!"
	#./parser test_script "parser_test.ppm"
	#echo "Seems to work! Saved to parser_test.ppm"
	echo "Running test script."
	./parser script

	#echo "Generating GIF \"Valley Of The Worm"\
	#./bezier
	make convert
	#convert -delay 10 -loop 0 bezier_*[048].png bezier.gif
	#echo "Saved to bezier.gif"
	make clean
	rm *.png
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
