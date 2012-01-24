ifeq ($(shell uname), Linux)
  One = -I/usr/X11R6/include -L/usr/X11R6/bin
  Two = -lglut -lGL
  Three = ./cubior
else 
  One = -I"C:\MinGW\freeglut\include" -L"C:\MinGW\freeglut\lib"
  Two = -lfreeglut -lopengl32 -Wl,--subsystem,windows
  Three = ./cubior.exe
endif

all:
	g++ gameplay.cpp flatRender.cpp textRender.cpp visuals.cpp $(One) -o cubior cubior.cpp $(Two) && $(Three)