all:
	g++ -c -O2 Mandelbrot.cpp -DDRAW -mavx2 -fno-omit-frame-pointer -o calculation.o
	g++ calculation.o -o mndl -lsfml-graphics -lsfml-window -lsfml-system
	./mndl

clear:
	rm -f *.o
