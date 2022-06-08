CXX = g++
CXXFLAGS = -Wall

main: 
	$(CXX) $(CXXFLAGS) main.cpp -o output

run:
	./output