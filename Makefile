CXX_FLAGS= -std=c++0x
CXX_FLAGS_DEBUG= $(CXX_FLAGS) -g
CXX=g++

TARGET=distance-fields
SRC= main.cpp

default:
	$(CXX) $(SRC) $(CXX_FLAGS) -o $(TARGET)
#	./$(TARGET)

debug:
	$(CXX) $(SRC) $(CXX_FLAGS_DEBUG) -o $(TARGET)
