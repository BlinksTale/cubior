ifeq ($(shell uname), Linux)
  Graphics = -I/usr/X11R6/include -L/usr/X11R6/bin -lglut -lGL -lGLU
  Exe = cubiorTest
else ifeq ($(shell uname), Darwin) # Darwin = Mac
  Graphics = -framework OpenGL -framework GLUT
  Exe = cubiorTest
else 
  Graphics = -I"C:\MinGW\freeglut\include" -L"C:\MinGW\freeglut\lib" -lfreeglut -lopengl32 -Wl,--subsystem,windows
  Exe = cubiorTest.exe
endif

all: ../bin/cubeObj.o ../bin/cubiorTest.o ../bin/gameplay.o
	g++ ../bin/cubeObj.o ../bin/cubiorTest.o ../bin/gameplay.o -o ../bin/cubiorTest && ../bin/$(Exe)

../bin/cubiorTest.o: cubiorTest.cpp
	g++ -c cubiorTest.cpp -o ../bin/cubiorTest.o

../bin/cubeObj.o: ../src/cubeObj.cpp ../src/gameplay.o
	g++ -c ../src/cubeObj.cpp -o ../bin/cubeObj.o

../bin/gameplay.o: ../src/gameplay.cpp
	g++ -c ../src/gameplay.cpp -o ../bin/gameplay.o
