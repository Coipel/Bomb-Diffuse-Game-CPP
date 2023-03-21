CC=g++
FLAGS=-Wall

EXE=bomb_diffuse_game
SRC_FILES=WireModule.cpp
SRC_FILES:=$(foreach src_file, $(SRC_FILES), ./src/$(src_file))
LIBS=sfml-graphics sfml-window sfml-system sfml-audio 
LIBS:=$(foreach lib, $(LIBS), -l$(lib))

all: game run

game: main.cpp $(FILES)
	$(CC) $(FLAGS) -o $(EXE) -I./include main.cpp $(SRC_FILES) $(LIBS)
	
clean:
	rm $(EXE)

run:
	./$(EXE)
