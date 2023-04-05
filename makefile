all:
	g++ -c Mandelbrot.cpp -mavx2 -fno-omit-frame-pointer -o calculation.o
	g++ calculation.o -o mndl -lsfml-graphics -lsfml-window -lsfml-system
	./mndl

clear:
	rm -f *.o
