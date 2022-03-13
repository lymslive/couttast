CXX = g++
CXXFLAGS = -fPIC
# CXXFLAGS += -Dprivate=public -Dprotected=pblic

SRC_DIR = ../basiccpp
SRC_ALL = $(wildcard $(SRC_DIR)/*.cpp)
SRC = $(notdir $(SRC_ALL))
SRC += main.cpp
OBJ = $(patsubst %.cpp, %.o, $(SRC))

vpath %.cpp $(SRC_DIR)

INC_DIR = ../../include
INC = -I$(INC_DIR)
# LIB_DIR = ../../lib
# LIB = -L$(LIB_DIR) -ltast_main
CXXFLAGS += $(INC)

TARGET = ./tast-basiccpp.exe

.PHONY : all clean rebuild echo

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB)

# implict rule ok
# $(OBJ) : %.o : %.cpp

clean:
	rm -rf $(TARGET) $(OBJ)

rebuild: clean all

echo:
	@echo SRC
	@echo $(SRC)
	@echo OBJ
	@echo $(OBJ)
