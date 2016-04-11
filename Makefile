all: pinball

pinball: pinball.cpp
	g++ pinball.cpp libggfonts.a -Wall -Wextra -opinball -lX11 -lGL -lGLU -lm -lrt

clean:
	rm -f pinball
	rm -f *.o

