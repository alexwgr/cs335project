all: pinball
pinball: pinball.cpp ppm.cpp alexR.cpp omarO.cpp hseid.cpp
	g++ pinball.cpp ppm.cpp alexR.cpp omarO.cpp vector.cpp libggfonts.a hseid.cpp /usr/lib/x86_64-linux-gnu/libopenal.so /usr/lib/libalut.so -Wall -Wextra -opinball -lX11 -lGL -lGLU -lm -lrt
clean:
	rm -f pinball
	rm -f *.o
