# c, cpp
FILE_ENDING := c

CC := $(if $(filter cpp, $(FILE_ENDING)), g++, gcc)
RELEASE_CCARGS := -Wall -Werror -Wpedantic
CCARGS := -lSDL2 -lGL -ldl -lm

.PHONY: clean
all: clean compile run

compile:
	$(CC) src/*.$(FILE_ENDING) -o build/main -I./src/include $(CCARGS)

release:
	$(CC) src/*.$(FILE_ENDING) -o build/main -I./src/include/ $(RELEASE_CCARGS) $(CCARGS)

run:
	./build/main

bear:
	bear -- make

clean:
	rm -rf build
	mkdir build
