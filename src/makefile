ifeq ($(shell uname), Linux)
  Include = -I/usr/X11R6/include -L/usr/X11R6/bin -lglut -lGL -lGLU
  Exe = ./cubior
else ifeq ($(shell uname), Darwin) # Darwin = Mac
  Include = -framework OpenGL -framework GLUT
  Exe = ./cubior
else 
  Include = -I"C:\MinGW\freeglut\include" -L"C:\MinGW\freeglut\lib" -lfreeglut -lopengl32 -Wl,--subsystem,windows
  Exe = ./cubior.exe
endif

all: 
	g++ cubior.cpp visuals.cpp flatRender.cpp gameplay.cpp keyboard.cpp textRender.cpp $(Include) -o cubior && $(Exe)
	

