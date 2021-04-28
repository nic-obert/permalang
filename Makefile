
CC=g++
STD=c++17

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find headers -name "*.hh")

HH=headers/pch.hh
PCH=$(HH).pch

BUILD_DIR=build

C_FLAGS=-std=$(STD) -I headers


all: build


build: $(PCH) $(SOURCES)
	$(CC) $(C_FLAGS) $(SOURCES) -o $(BUILD_DIR)/perma

builddb: $(PCH) $(SOURCES)
	$(CC) -g $(C_FLAGS) $(SOURCES) -o $(BUILD_DIR)/perma


$(PCH): $(HH)
	$(CC) $(C_FLAGS) $(HH) -o $(PCH)


clean:
	rm -f headers/*.pch 
	rm -f $(BUILD_DIR)/*

