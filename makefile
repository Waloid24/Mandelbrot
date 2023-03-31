all:
	g++ -c calculation.cpp -mavx2 -o calculation.o
	g++ calculation.o -o mndl -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	./mndl

clear:
	rm -f *.o
