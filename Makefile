
CC=g++
STD=c++17

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find headers -type f \( -name "*.hh" ! -name "pch.hh" \))

HH=headers/pch.hh
PCH=$(HH).pch

TARGET=target/perma


C_FLAGS=-std=$(STD) -I headers
LINKS=-ltimerpp -largparser
WARNINGS=-Wall -Wno-switch -Wno-reorder
WARNINGS_ALL=-Wall
COMMON_ARGS=$(C_FLAGS) $(SOURCES) $(LINKS)


all: build


build: $(PCH) $(SOURCES) $(HEADERS)
	$(CC) $(WARNINGS) $(COMMON_ARGS) -o $(TARGET)


builddb: $(PCH) $(SOURCES) $(HEADERS)
	$(CC) -g $(WARNINGS) $(COMMON_ARGS) -o $(TARGET)


builddb-nw: $(PCH) $(SOURCES) $(HEADERS)
	$(CC) -g $(COMMON_ARGS) -o $(TARGET)


build-wall: $(PCH) $(SOURCES) $(HEADERS)
	$(CC) $(WARNINGS_ALL) $(COMMON_ARGS) -o $(TARGET)


builddb-wall: $(PCH) $(SOURCES) $(HEADERS)
	$(CC) -g $(WARNINGS_ALL) $(COMMON_ARGS) -o $(TARGET)


$(PCH): $(HH)
	echo "Recompiling headers"
	$(CC) $(C_FLAGS) $(WARNINGS) $(HH) -o $(PCH)


clean:
	rm -f headers/*.pch 
	rm -f $(BUILD_DIR)/*

