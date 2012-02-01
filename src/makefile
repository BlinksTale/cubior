ifeq ($(shell uname), Linux)
  One = -I/usr/X11R6/include -L/usr/X11R6/bin -lglut -lGL -lGLU
  Exe = ./cubior
else ifeq ($(shell uname), Darwin) # Darwin = Mac
  One = -framework OpenGL -framework GLUT
  Exe = ./cubior
else 
  One = -I"C:\MinGW\freeglut\include" -L"C:\MinGW\freeglut\lib" -lfreeglut -lopengl32 -Wl,--subsystem,windows
  Exe = ./cubior.exe
endif

all:
	g++ keyboard.cpp gameplay.cpp flatRender.cpp textRender.cpp visuals.cpp $(One) -o cubior cubior.cpp && $(Exe)
