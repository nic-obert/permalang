
CC=g++
STD=c++17

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find headers -type f \( -name "*.hh" ! -name "pch.hh" \))

HH=headers/pch.hh
PCH=$(HH).pch

BUILD_DIR=build


C_FLAGS=-std=$(STD) -I headers
WARINGS=-Wall -Wno-switch
WARNINGS_ALL=-Wall


all: build


rules: $(PCH) $(SOURCES) $(HEADERS)


build: rules
	$(CC) $(C_FLAGS) $(WARINGS) $(SOURCES) -o $(BUILD_DIR)/perma

builddb: rules
	$(CC) -g $(C_FLAGS) $(WARINGS) $(SOURCES) -o $(BUILD_DIR)/perma

builddb-nw: rules
	$(CC) -g $(C_FLAGS_NW) $(SOURCES) -o $(BUILD_DIR)/perma

build-wall: rules
	$(CC) $(C_FLAGS) $(WARINGS_ALL) $(SOURCES) -o $(BUILD_DIR)/perma


$(PCH): $(HH)
	echo "Recompiling headers"
	$(CC) $(C_FLAGS) $(WARINGS) $(HH) -o $(PCH)


clean:
	rm -f headers/*.pch 
	rm -f $(BUILD_DIR)/*

