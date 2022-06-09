CXX = g++
CXXFLAGS = -Wall

main: Line.o main.cpp
	$(CXX) $(CXXFLAGS) Line.o main.cpp -o output

Line.o: Line.h Line.cpp
	$(CXX) $(CXXFLAGS) -c Line.cpp

run:
	./output