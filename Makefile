ifeq ($(shell uname), Linux)
  Graphics = -I/usr/X11R6/include -L/usr/X11R6/bin -lglut -lGL -lGLU
  Audio = -Wall -lopenal -lalut #-Wall -lopenal -lalut
  Exe = cubior
else ifeq ($(shell uname), Darwin) # Darwin = Mac
  Graphics = -framework OpenGL -framework GLUT -I./include -L./lib -framework sfml-graphics -framework sfml-system -framework sfml-window
  Audio = -I./include -L./lib -framework sfml-audio
  Exe = cubior
else 
  #Graphics = -I"C:\MinGW\freeglut\include" -L"C:\MinGW\freeglut\lib" -lfreeglut -lglu32 -lopengl32 -Wl,--subsystem,windows -static
  Graphics = -I ".\include" -L ".\lib" -lfreeglut -lglu32  -lopengl32 -Wl,--subsystem,windows -static
  Audio = #update later when on Win
  Exe = cubior.exe
endif

AllFiles = bin/cubiorObj.o bin/goalObj.o bin/cubeObj.o bin/visuals.o bin/flatRender.o bin/textRender.o bin/cubiorShape.o bin/goalShape.o bin/cubeShape.o bin/gameplay.o bin/keyboard.o bin/collision.o bin/cameraObj.o bin/trackerObj.o bin/mapReader.o bin/map.o bin/sfx.o bin/music.o bin/lodepng.o bin/image.o

all: bin/cubior.o bin/cubiorTest.o
	g++ $(AllFiles) bin/cubior.o $(Graphics) $(Audio) -o bin/cubior && bin/$(Exe)

bin/cubiorTest.o: bin/gameplay.o bin/visuals.o bin/cubior.o
	g++ -c test/cubiorTest.cpp -o bin/cubiorTest.o

bin/cubior.o: bin/visuals.o bin/gameplay.o
	g++ -c src/cubior.cpp -o bin/cubior.o

.PHONY: clean
clean:
	rm ./bin/*



###########
# VISUALS #
###########

bin/cubeShape.o: src/cubeShape.cpp
	g++ $(Graphics) -c src/cubeShape.cpp -o bin/cubeShape.o

bin/cubiorShape.o: src/cubiorShape.cpp bin/cubeShape.o bin/gameplay.o
	g++ $(Graphics) -c src/cubiorShape.cpp -o bin/cubiorShape.o

bin/goalShape.o: src/goalShape.cpp bin/cubeShape.o
	g++ $(Graphics) -c src/goalShape.cpp -o bin/goalShape.o

bin/visuals.o: src/visuals.cpp bin/flatRender.o bin/textRender.o bin/image.o
	g++ -c src/visuals.cpp -o bin/visuals.o

bin/flatRender.o: src/flatRender.cpp bin/gameplay.o bin/keyboard.o bin/cubeShape.o bin/cubiorShape.o bin/goalShape.o bin/sfx.o bin/music.o bin/lodepng.o
	g++ $(Audio) $(Graphics) -c src/flatRender.cpp -o bin/flatRender.o

bin/textRender.o: src/textRender.cpp
	g++ -c src/textRender.cpp -o bin/textRender.o

bin/lodepng.o: src/lodepng.cpp
	g++ -c src/lodepng.cpp -o bin/lodepng.o

bin/image.o: src/image.cpp bin/lodepng.o
	g++ $(Graphics) -c src/image.cpp -o bin/image.o

#########
# AUDIO #
#########

bin/sfx.o: src/sfx.cpp bin/gameplay.o
	g++ $(Audio) -c src/sfx.cpp -o bin/sfx.o
	# works w/o audio actually, but this helps show where libs are used

bin/music.o: src/sfx.cpp bin/gameplay.o
	g++ $(Audio) -c src/music.cpp -o bin/music.o
	# works w/o audio actually, but this helps show where libs are used

############
# GAMEPLAY #
############

bin/gameplay.o: src/gameplay.cpp bin/cubiorObj.o bin/cubeObj.o bin/cubiorObj.o bin/collision.o bin/goalObj.o bin/cameraObj.o bin/mapReader.o
	g++ -c src/gameplay.cpp -o bin/gameplay.o

bin/collision.o: src/collision.cpp bin/cubeObj.o
	g++ -c src/collision.cpp -o bin/collision.o

bin/cubeObj.o: src/cubeObj.cpp bin/gameplay.o
	g++ -c src/cubeObj.cpp -o bin/cubeObj.o

bin/cubiorObj.o: src/cubiorObj.cpp bin/cubeObj.o bin/gameplay.o
	g++ -c src/cubiorObj.cpp -o bin/cubiorObj.o

bin/goalObj.o: src/goalObj.cpp bin/cubeObj.o bin/gameplay.o
	g++ -c src/goalObj.cpp -o bin/goalObj.o

bin/cameraObj.o: src/cameraObj.cpp bin/gameplay.o bin/trackerObj.o
	g++ -c src/cameraObj.cpp -o bin/cameraObj.o

bin/trackerObj.o: src/trackerObj.cpp bin/cubeObj.o
	g++ -c src/trackerObj.cpp -o bin/trackerObj.o

bin/mapReader.o: src/mapReader.cpp bin/gameplay.o bin/map.o
	g++ -c src/mapReader.cpp -o bin/mapReader.o

bin/map.o: src/map.cpp bin/cubeObj.o bin/gameplay.o
	g++ -c src/map.cpp -o bin/map.o


#########
# INPUT #
#########

bin/keyboard.o: src/keyboard.cpp
	g++ $(Graphics) -c src/keyboard.cpp -o bin/keyboard.o

