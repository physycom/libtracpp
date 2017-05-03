INC = -Ijsoncons/src/

all:
	$(CXX) -std=c++11 $(INC) -o test.exe test.cpp
