ifeq ($(shell uname), Linux)
  Graphics = -I/usr/X11R6/include -L/usr/X11R6/bin -lglut -lGL -lGLU
  Exe = cubior
else ifeq ($(shell uname), Darwin) # Darwin = Mac
  Graphics = -framework OpenGL -framework GLUT
  Exe = cubior
else 
  Graphics = -I"C:\MinGW\freeglut\include" -L"C:\MinGW\freeglut\lib" -lfreeglut -lopengl32 -Wl,--subsystem,windows
  Exe = cubior.exe
endif

all: bin/cubior.o
	g++ bin/cubeObj.o bin/visuals.o bin/flatRender.o bin/textRender.o bin/cubeShape.o bin/gameplay.o bin/keyboard.o bin/cubior.o $(Graphics) -o bin/cubior && bin/$(Exe)

bin/cubior.o: bin/visuals.o bin/gameplay.o
	g++ -c src/cubior.cpp -o bin/cubior.o

bin/cubeShape.o: src/cubeShape.cpp
	g++ -c src/cubeShape.cpp -o bin/cubeShape.o

bin/visuals.o: src/visuals.cpp bin/flatRender.o bin/textRender.o
	g++ -c src/visuals.cpp -o bin/visuals.o

bin/flatRender.o: src/flatRender.cpp bin/gameplay.o bin/keyboard.o bin/cubeShape.o
	g++ -c src/flatRender.cpp -o bin/flatRender.o

bin/gameplay.o: src/gameplay.cpp bin/cubeObj.o
	g++ -c src/gameplay.cpp -o bin/gameplay.o

bin/keyboard.o: src/keyboard.cpp
	g++ -c src/keyboard.cpp -o bin/keyboard.o

bin/textRender.o: src/textRender.cpp
	g++ -c src/textRender.cpp -o bin/textRender.o

bin/cubeObj.o: src/cubeObj.cpp
	g++ -c src/cubeObj.cpp -o bin/cubeObj.o

