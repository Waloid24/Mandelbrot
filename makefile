all:
	g++ -c calculation.cpp -o calculation.o
	g++ calculation.o -o mndl -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	./mndl

no_avx:
	g++ -c calculation.cpp -o calculation.o
	g++ calculation.o -o mndl -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -DNO_AVX
	./mndl
clear:
	rm -f *.o
