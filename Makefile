
CC=g++
STD=c++17

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find headers -name "*.hh")
PCH=headers/pch.hh

BUILD_DIR=build


all: build


build: precompile_headers $(SOURCES)
	$(CC) -std=$(STD) -I headers $(SOURCES) -o $(BUILD_DIR)/perma

builddb: $(HEADERS) precompile_headers $(SOURCES)
	$(CC) -std=$(STD) -g -I headers $(SOURCES) -o $(BUILD_DIR)/perma


precompile_headers: $(HEADERS)
	$(CC) -std=$(STD) -I headers $(PCH) -o $(PCH).pch


clean:
	rm -f headers/*.pch 
	rm -f $(BUILD_DIR)/*

