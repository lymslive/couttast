.PHONY : all dir

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

lib/libtast_main.a : build/tast_main.o
	$(AR) $@ $^

bin/tast_drive : build/tast_drive.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIBRARY)
