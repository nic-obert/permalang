
CC=g++
STD=c++17

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find headers -name "*.hh")

HH=headers/pch.hh
PCH=$(HH).pch

BUILD_DIR=build


all: build


build: $(PCH) $(SOURCES)
	$(CC) -std=$(STD) -I headers $(SOURCES) -o $(BUILD_DIR)/perma

builddb: $(PCH) $(SOURCES)
	$(CC) -std=$(STD) -g -I headers $(SOURCES) -o $(BUILD_DIR)/perma


$(PCH): $(HEADERS)
	$(CC) -std=$(STD) -I headers $(HH) -o $(PCH)


clean:
	rm -f headers/*.pch 
	rm -f $(BUILD_DIR)/*

