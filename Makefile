
CC=g++

sources=$(shell find src -name "*.cpp")

all: build

build: $(sources)
	$(CC) -std=c++11 -I headers $(sources) -o perma

builddb: $(sources)
	$(CC) -std=c++11 -g -I headers $(sources) -o perma