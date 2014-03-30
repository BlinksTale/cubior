# LINUX
ifeq ($(shell uname), Linux)
  Compiler = clang++ -std=c++0x # and 32 bit builds on 64 bit machines use -m32
  GraphicsInlude = -I/usr/X11R6/include
  GraphicsLinker = -L/usr/X11R6/bin -L/usr/local/lib -lglee -lglut -lGL -lGLU -lsfml-graphics -lsfml-window -lsfml-system
  Audio = -Wall -lopenal -lalut -L/usr/local/lib -lsfml-audio #-Wall -lopenal -lalut
  Networking = -L/usr/local/lib -lsfml-network
  Exe = cubior

# MAC
else ifeq ($(shell uname), Darwin) # Darwin = Mac
  Compiler = clang++ -std=c++0x
  GraphicsInclude = -I./include
  GraphicsLinker = -framework OpenGL -framework GLUT -L./lib -framework sfml-graphics -framework sfml-system -framework sfml-window
  Audio = -I./include -L./lib -framework sfml-audio
  Networking = -framework sfml-network
  Exe = cubior

# WINDOWS
else 
  Compiler = g++ -std=c++0x
  #Graphics = -I"C:\MinGW\freeglut\include" -L"C:\MinGW\freeglut\lib" -lfreeglut -lglu32 -lopengl32 -Wl,--subsystem,windows -static
  GraphicsInclude = -I ".\include"
  GraphicsLinker = -L ".\lib" -lfreeglut -lglu32  -lopengl32 -Wl,--subsystem,windows -static
  Audio = #update later when on Win
  Networking = -#update later on win
  Exe = cubior.exe

# ALL
endif



AllFiles = bin/cubiorObj.o bin/goalObj.o bin/cubeObj.o bin/visuals.o bin/flatRender.o bin/textRender.o bin/cubiorShape.o bin/goalShape.o bin/cubeShape.o bin/gameplay.o bin/keyboard.o bin/collision.o bin/cameraObj.o bin/trackerObj.o bin/mapReader.o bin/map.o bin/sfx.o bin/music.o bin/lodepng.o bin/image.o bin/creditsReader.o bin/networking.o

all: bin/cubior.o bin/cubiorTest.o
	$(Compiler) $(AllFiles) bin/cubior.o $(GraphicsLinker) $(Audio) $(Networking) -o bin/cubior && bin/$(Exe)

bin/cubiorTest.o: bin/gameplay.o bin/visuals.o bin/cubior.o
	$(Compiler) -c test/cubiorTest.cpp -o bin/cubiorTest.o

bin/cubior.o: bin/visuals.o bin/gameplay.o
	$(Compiler) -c src/cubior.cpp -o bin/cubior.o

.PHONY: clean
clean:
	rm ./bin/*



###########
# VISUALS #
###########

bin/cubeShape.o: src/cubeShape.cpp
	$(Compiler) $(GraphicsInclude) -c src/cubeShape.cpp -o bin/cubeShape.o

bin/cubiorShape.o: src/cubiorShape.cpp bin/cubeShape.o bin/gameplay.o
	$(Compiler) $(GraphicsInclude) -c src/cubiorShape.cpp -o bin/cubiorShape.o

bin/goalShape.o: src/goalShape.cpp bin/cubeShape.o
	$(Compiler) $(GraphicsInclude) -c src/goalShape.cpp -o bin/goalShape.o

bin/visuals.o: src/visuals.cpp bin/flatRender.o bin/textRender.o bin/image.o
	$(Compiler) -c src/visuals.cpp -o bin/visuals.o

bin/flatRender.o: src/flatRender.cpp bin/gameplay.o bin/keyboard.o bin/cubeShape.o bin/cubiorShape.o bin/goalShape.o bin/sfx.o bin/music.o bin/creditsReader.o bin/lodepng.o
	$(Compiler) $(GraphicsInclude) -c src/flatRender.cpp -o bin/flatRender.o

bin/textRender.o: src/textRender.cpp
	$(Compiler) -c src/textRender.cpp -o bin/textRender.o

bin/lodepng.o: src/lodepng.cpp
	$(Compiler) -c src/lodepng.cpp -o bin/lodepng.o

bin/image.o: src/image.cpp bin/lodepng.o
	$(Compiler) $(GraphicsInclude) -c src/image.cpp -o bin/image.o

#########
# AUDIO #
#########

bin/sfx.o: src/sfx.cpp bin/gameplay.o
	$(Compiler) -c src/sfx.cpp -o bin/sfx.o
	# works w/o audio actually, but this helps show where libs are used

bin/music.o: src/sfx.cpp bin/gameplay.o
	$(Compiler) -c src/music.cpp -o bin/music.o
	# works w/o audio actually, but this helps show where libs are used

############
# GAMEPLAY #
############

bin/gameplay.o: src/gameplay.cpp bin/cubiorObj.o bin/cubeObj.o bin/cubiorObj.o bin/collision.o bin/goalObj.o bin/cameraObj.o bin/mapReader.o bin/networking.o
	$(Compiler) -c src/gameplay.cpp -o bin/gameplay.o

bin/collision.o: src/collision.cpp bin/cubeObj.o
	$(Compiler) -c src/collision.cpp -o bin/collision.o

bin/cubeObj.o: src/cubeObj.cpp bin/gameplay.o
	$(Compiler) -c src/cubeObj.cpp -o bin/cubeObj.o

bin/cubiorObj.o: src/cubiorObj.cpp bin/cubeObj.o bin/gameplay.o
	$(Compiler) -c src/cubiorObj.cpp -o bin/cubiorObj.o

bin/goalObj.o: src/goalObj.cpp bin/cubeObj.o bin/gameplay.o
	$(Compiler) -c src/goalObj.cpp -o bin/goalObj.o

bin/cameraObj.o: src/cameraObj.cpp bin/gameplay.o bin/trackerObj.o
	$(Compiler) -c src/cameraObj.cpp -o bin/cameraObj.o

bin/trackerObj.o: src/trackerObj.cpp bin/cubeObj.o
	$(Compiler) -c src/trackerObj.cpp -o bin/trackerObj.o

bin/mapReader.o: src/mapReader.cpp bin/gameplay.o bin/map.o
	$(Compiler) -c src/mapReader.cpp -o bin/mapReader.o

bin/map.o: src/map.cpp bin/cubeObj.o bin/gameplay.o
	$(Compiler) -c src/map.cpp -o bin/map.o

bin/creditsReader.o: src/creditsReader.cpp bin/gameplay.o
	$(Compiler) -c src/creditsReader.cpp -o bin/creditsReader.o


#########
# INPUT #
#########

bin/keyboard.o: src/keyboard.cpp
	$(Compiler) $(GraphicsInclude) -c src/keyboard.cpp -o bin/keyboard.o

##############
# NETWORKING #
##############

bin/networking.o: src/networking.cpp
	$(Compiler) -c src/networking.cpp -o bin/networking.o

