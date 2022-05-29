.PHONY : all dir clean rebuild example install uninstall

TAST_LIB = lib/libtast_main.a
TAST_BIN = bin/tast_drive
all: dir $(TAST_LIB) $(TAST_BIN)

dir:
	@mkdir -p build lib bin

AR = ar -crs
CXX = g++
CXXFLAGS = -fPIC

INCLUDE = -I./include
LIBRARY = -ldl

SRC_DIR = ./src
SRC_ALL = $(wildcard $(SRC_DIR)/*.cpp)
SRC_NODIR = $(notdir $(SRC_ALL))
OBJ_NODIR = $(SRC_NODIR:%.cpp=%.o)

OBJ_DIR = ./build
OBJ_ALL = $(addprefix $(OBJ_DIR)/, $(OBJ_NODIR))

$(OBJ_ALL): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

$(TAST_LIB) : build/tast_main.o
	$(AR) $@ $^

$(TAST_BIN) : build/tast_drive.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBRARY)

example:
	make -C example/basiccpp-umain/
	make -C example/basiccpp-uliba/
	make -C example/basiccpp-udyso/
	make -C example/tastself/

clean:
	rm $(TAST_LIB) $(TAST_BIN) $(OBJ_ALL)

rebuild: clean all

PREFIX ?= $(HOME)
INSTALL_INC = $(PREFIX)/include/couttast
INSTALL_LIB = $(PREFIX)/lib
install:
	@mkdir -p $(INSTALL_INC)
	cp -u include/*.hpp $(INSTALL_INC)
	@mkdir -p $(INSTALL_LIB)
	cp -u $(TAST_LIB) $(INSTALL_LIB)

uninstall:
	rm -rf $(INSTALL_INC)
	rm -rf $(INSTALL_LIB)/$(notdir $(TAST_LIB))

