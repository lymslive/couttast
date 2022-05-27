CXX = g++ -std=c++11

TARGET = tastself.exe
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))
INC += -I../../include

CXXFLAGS += -g
CXXFLAGS += $(INC)

$(TARGET) : $(OBJ)
	$(CXX) -o $@ $^

clean :
	rm -rf $(OBJ) $(TARGET)

run : $(TARGET)
	./$(TARGET)
