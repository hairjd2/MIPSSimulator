CXX = g++
CXXFLAGS = -Wall

main: Line.o main.cpp
	$(CXX) $(CXXFLAGS) Line.o Register.o main.cpp -o output

Line.o: Line.h Line.cpp
	$(CXX) $(CXXFLAGS) -c Line.cpp

Register.o: Register.h Register.cpp
	$(CXX) $(CXXFLAGS) -c Register.cpp

run:
	./output