CXX_FLAGS= -std=c++0x

TARGET=distance-fields
SRC= main.cpp

default:
	g++ $(SRC) $(CXX_FLAGS) -o $(TARGET)
#	./$(TARGET)
