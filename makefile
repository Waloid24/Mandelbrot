all:
	g++ -c calculation.cpp -mavx2 -DAVX_ON -DDRAW -O1 -o calculation.o
	g++ calculation.o -o mndl -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	./mndl

clear:
	rm -f *.o
