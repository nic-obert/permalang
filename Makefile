
CC=g++
STD=c++17

sources=$(shell find src -name "*.cpp")

all: build

build: $(sources)
	$(CC) -std=$(STD) -I headers $(sources) -o perma

builddb: $(sources)
	$(CC) -std=$(STD) -g -I headers $(sources) -o perma

clang: $(sources)
	clang++ -std=$(STD) -I headers $(sources) -o perma

clangdb: $(sources)
	clang++ -std=$(STD) -g -I headers $(sources) -o perma

