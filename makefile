.PHONY : all dir clean rebuild example install uninstall

TINYTAST_LIB = lib/libtinytast.a
COUTTAST_LIB = lib/libcouttast.a
TASTDRIVE_BIN = bin/tast_drive
UTEST_BIN = bin/utCoutTast
UTEST_MIN_BIN = bin/utCxx98Tast

TARGET_ALL = $(TINYTAST_LIB) $(COUTTAST_LIB) $(TASTDRIVE_BIN) $(UTEST_BIN) $(UTEST_MIN_BIN)
all: dir $(TARGET_ALL)

dir:
	@mkdir -p obj lib bin

AR = ar -crs
CXX = g++
CXXFLAGS = -std=c++11 -fPIC

INCLUDE = -I./include
LIBRARY = -ldl

# compile src/
SRC_DIR = ./src
SRC_ALL = $(wildcard $(SRC_DIR)/*.cpp)
SRC_NODIR = $(notdir $(SRC_ALL))
OBJ_NODIR = $(SRC_NODIR:%.cpp=%.o)

OBJ_DIR = ./obj
OBJ_ALL = $(addprefix $(OBJ_DIR)/, $(OBJ_NODIR))

$(OBJ_ALL): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

$(TINYTAST_LIB) : $(OBJ_DIR)/tinytast.o
	$(AR) $@ $^

$(TASTDRIVE_BIN) : $(OBJ_DIR)/tast_drive.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBRARY)

SEPARATE_OBJ = $(OBJ_DIR)/tinytast.o $(OBJ_DIR)/tast_drive.o
COUTTAST_OBJ = $(filter-out $(SEPARATE_OBJ), $(OBJ_ALL))
$(COUTTAST_LIB) : $(COUTTAST_OBJ)
	$(AR) $@ $^

# compile utest/
UTEST_DIR = ./utest
UTEST_SRC = $(wildcard $(UTEST_DIR)/*.cpp)
UTEST_SRC_NODIR = $(notdir $(UTEST_SRC))
UTEST_OBJ_NODIR = $(UTEST_SRC_NODIR:%.cpp=%.o)

UTEST_OBJ_ALL = $(addprefix $(OBJ_DIR)/, $(UTEST_OBJ_NODIR))
$(UTEST_OBJ_ALL): $(OBJ_DIR)/%.o : $(UTEST_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

$(UTEST_BIN) : $(UTEST_OBJ_ALL)
	$(CXX) -o $@ $(CXXFLAGS) $^ -lcouttast -L./lib

$(UTEST_MIN_BIN) : $(UTEST_DIR)/test-tinytast.cpp
	$(CXX) -o $@ -std=c++98 $^ -DUSE_TINY_MAIN $(INCLUDE)

example:
	make -C example/basiccpp-umain/
	make -C example/basiccpp-uliba/
	make -C example/basiccpp-udyso/
	make -C example/tastself/

clean:
	rm $(TARGET_ALL) $(OBJ_ALL) $(UTEST_OBJ_ALL)

rebuild: clean all

test: $(UTEST_BIN) $(UTEST_MIN_BIN)
	$(UTEST_MIN_BIN)
	cd $(UTEST_DIR) && ../$(UTEST_BIN)

PREFIX ?= $(HOME)
INSTALL_HEADER = include/*.hpp include/*.h
INSTALL_INC = $(PREFIX)/include/couttast
INSTALL_LIB = $(PREFIX)/lib
install:
	@mkdir -p $(INSTALL_INC)
	cp -u $(INSTALL_HEADER) $(INSTALL_INC)
	@mkdir -p $(INSTALL_LIB)
	cp -u $(TINYTAST_LIB) $(COUTTAST_LIB) $(INSTALL_LIB)

uninstall:
	rm -rf $(INSTALL_INC)
	rm -rf $(INSTALL_LIB)/$(notdir $(TINYTAST_LIB))
	rm -rf $(INSTALL_LIB)/$(notdir $(COUTTAST_LIB))

cmake:
	@mkdir -p build
	cd build && cmake .. && make && make test

cmake.debug:
	cd build && make clean && cmake .. -DCMAKE_BUILD_TYPE=Debug && make && make test
