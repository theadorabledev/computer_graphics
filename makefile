all: picmaker
picmaker: picmaker.c
	gcc -o picmaker picmaker.c -lm
run:
	./picmaker
display:
	display *.png
convert:
	convert *.ppm -set filename:base "%[basename]" "%[filename:base].png"
clean:
	rm -f *~
	rm -f *.o
	rm -f *.ppm
