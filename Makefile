
CC=g++
STD=c++17

SRC_DIR=src
HEADERS_DIR=headers
LIB_DIR=lib
TARGET_DIR=target


SOURCES=$(shell find $(SRC_DIR) -type f -name "*.cpp")


HEADERS=$(shell find $(HEADERS_DIR) -type f \( -name "*.hh" ! -name "pch.hh" \))

HH=$(HEADERS_DIR)/pch.hh
PCH=$(HH).pch


TARGET=$(TARGET_DIR)/pcc


C_FLAGS=-std=$(STD) -I $(HEADERS_DIR)

LINKS=-ltimerpp -largparser

WARNINGS=-Wall -Wno-switch -Wno-reorder
WARNINGS_ALL=-Wall

COMMON_ARGS=$(C_FLAGS) $(SOURCES) $(LINKS)
COMMON_ARGS_TEST=$(C_FLAGS) $(SOURCES) $(TEST_CPP) $(LINKS)


INSTALL_DIR_LIB=/usr/lib/permalang
INSTALL_PCC=/usr/bin/pcc


all: build


$(TARGET_DIR):
	mkdir $(TARGET_DIR)


build: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) $(WARNINGS) $(COMMON_ARGS) -o $(TARGET)
	@echo "Build successful"


builddb: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) -g $(WARNINGS) $(COMMON_ARGS_TEST) -o $(TARGET)


builddb-nw: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) -g $(COMMON_ARGS_TEST) -o $(TARGET_TEST)


build-wall: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) $(WARNINGS_ALL) $(COMMON_ARGS_TEST) -o $(TARGET)


builddb-wall: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) -g $(WARNINGS_ALL) $(COMMON_ARGS_TEST) -o $(TARGET)


$(PCH): $(HH)
	@echo "Compiling headers"
	$(CC) $(C_FLAGS) $(WARNINGS) $(HH) -o $(PCH)


clean:
	rm -f $(PCH)
	rm -f $(TARGET_DIR)/*


install: $(TARGET)
	sudo mkdir $(INSTALL_DIR_LIB)
	sudo cp -f $(LIB_DIR)/* $(INSTALL_DIR_LIB)
	sudo cp -f $(TARGET) $(INSTALL_PCC)
	@echo "Installed successfully"


uninstall:
	sudo rm -fr $(INSTALL_DIR_LIB)
	sudo rm -f $(INSTALL_PCC)
	@echo "Uninstalled successfully"

