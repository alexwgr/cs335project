all: pinball

pinball: pinball.cpp ppm.cpp alexR.cpp hseid.cpp
	g++ pinball.cpp ppm.cpp  alexR.cpp hseid.cpp vector.cpp libggfonts.a -Wall -Wextra -opinball -lX11 -lGL -lGLU -lm -lrt

clean:
	rm -f pinball
	rm -f *.o
