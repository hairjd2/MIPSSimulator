CXX = g++
CXXFLAGS = -Wall

main: Line.o IntRegister.o FloatRegister.o main.cpp
	$(CXX) $(CXXFLAGS) Line.o IntRegister.o FloatRegister.o main.cpp -o output

Line.o: Line.h Line.cpp
	$(CXX) $(CXXFLAGS) -c Line.cpp

IntRegister.o: IntRegister.h IntRegister.cpp
	$(CXX) $(CXXFLAGS) -c IntRegister.cpp

FloatRegister.o: FloatRegister.h FloatRegister.cpp
	$(CXX) $(CXXFLAGS) -c FloatRegister.cpp

run:
	./output

valgrind:
	valgrind ./output

gdb:
	gdb ./output