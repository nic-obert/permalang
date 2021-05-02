
CC=g++
STD=c++17

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find headers -type f \( -name "*.hh" ! -name "pch.hh" \))

HH=headers/pch.hh
PCH=$(HH).pch

BUILD_DIR=build

C_FLAGS=-std=$(STD) -I headers -Wall


all: build


build: $(PCH) $(SOURCES) $(HEADERS)
	$(CC) $(C_FLAGS) $(SOURCES) -o $(BUILD_DIR)/perma

builddb: $(PCH) $(SOURCES) $(HEADERS)
	$(CC) -g $(C_FLAGS) $(SOURCES) -o $(BUILD_DIR)/perma


$(PCH): $(HH)
	$(CC) $(C_FLAGS) $(HH) -o $(PCH)


clean:
	rm -f headers/*.pch 
	rm -f $(BUILD_DIR)/*

