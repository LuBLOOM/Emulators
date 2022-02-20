CC = gcc
SRC = $(wildcard src/*.c)
LINKER = -lSDL2 -lSDL2_image -lSDL2_ttf -lGLEW -lGL -LGLU
EXEC = ./bin/emud

$(EXEC) : $(SRC)
	$(CC) $^ -o $@ $(LINKER)

clean:
	rm $(EXEC)

