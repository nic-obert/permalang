
CC=g++
STD=c++17

SRC_DIR=src
HEADERS_DIR=headers
LIB_DIR=lib
TARGET_DIR=target
IMPL_DIR=impl


SOURCES=$(shell find $(SRC_DIR) -type f -name "*.cpp")
MEM_TEST_SRC=$(shell find $(SRC_DIR) -type f \( -name "*.cpp" ! -name "pcc.cpp" \))


HEADERS=$(shell find $(HEADERS_DIR) -type f \( -name "*.hh" ! -name "pch.hh" \))

HH=$(HEADERS_DIR)/pch.hh
PCH=$(HH).pch


TARGET=$(TARGET_DIR)/pcc


C_FLAGS=-std=$(STD) -I $(HEADERS_DIR)

LINKS=-ltimerpp -largparser

WARNINGS=-Wall -Wno-switch -Wno-reorder -Wconversion
WARNINGS_ALL=-Wall

COMMON_ARGS=$(C_FLAGS) $(SOURCES) $(LINKS)
COMMON_ARGS_TEST=$(C_FLAGS) $(SOURCES) $(LINKS)


INSTALL_DIR_LIB=/usr/lib/permalang
INSTALL_PCC=/usr/bin/pcc


all: builddb


$(TARGET_DIR):
	mkdir -p $@


$(TARGET): $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) $(COMMON_ARGS) -o $@
	@echo "Build successful"


release: $(TARGET)


builddb: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	time -f "\nTime elapsed: %E seconds" $(CC) -g $(WARNINGS) $(COMMON_ARGS_TEST) -o $(TARGET)


builddb-nw: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) -g $(COMMON_ARGS_TEST) -o $(TARGET_TEST)


build-wall: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) $(WARNINGS_ALL) $(COMMON_ARGS_TEST) -o $(TARGET)


builddb-wall: $(PCH) $(SOURCES) $(HEADERS) $(TARGET_DIR)
	$(CC) -g $(WARNINGS_ALL) $(COMMON_ARGS_TEST) -o $(TARGET)



$(PCH): $(HH)
	@echo "Compiling headers"
	$(CC) $(C_FLAGS) $(WARNINGS) $^ -o $@


# tests

memtest: $(PCH) $(MEM_TEST_SRC) $(HEADERS) test/memtest.cpp
	$(CC) -g $(WARNINGS) $(C_FLAGS) test/memtest.cpp $(MEM_TEST_SRC) $(LINKS) -o target/$@


tests:
	test/tester.py

run:
	$(TARGET) $(IMPL_DIR)/script.pf -v

runo:
	$(TARGET) $(IMPL_DIR)/script.pf -v -O

exe:
	$(TARGET) $(IMPL_DIR)/script.pf.pfx -x


clean:
	rm -f $(PCH)
	rm -f $(TARGET_DIR)/*
	find $(IMPL_DIR) -name "*.pfx" -type f -delete



install-pcc: $(TARGET)
	@echo "Installing pcc toolchain"
	sudo cp -f $^ $(INSTALL_PCC)


install-lib:
	@echo "Installing libraries"
	sudo mkdir -p $(INSTALL_DIR_LIB)
	sudo cp -f $(LIB_DIR)/* $(INSTALL_DIR_LIB)


install: install-lib install-pcc	
	@echo "Installed successfully"


uninstall:
	@echo "Uninstalling libraries"
	sudo rm -fr $(INSTALL_DIR_LIB)
	@echo "Uninstalling pcc toolchain"
	sudo rm -f $(INSTALL_PCC)
	@echo "Uninstalled successfully"


count:
	find . -name '*.cpp' -o -name '*.hh' | xargs wc -l | sort -nr

