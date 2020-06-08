main.o : main.c
	gcc -c main.c
	gcc -c funcoes.c

all: main.o
	gcc -o jogo main.o funcoes.o -lGL -lGLU -lglut -lSOIL -lm -lSDL2 -lSDL2_mixer

run: all
	./jogo

clean:
	rm *.o
