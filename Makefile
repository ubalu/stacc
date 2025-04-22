
CXXFLAGS=-xc++ -Wall -Wextra -Wpedantic -Werror -std=c++17 -g3 -ggdb

stacc: $(wildcard src/*.cpp src/*.hpp src/*.h)
	$(CXX) $(CXXFLAGS) -o stacc $(wildcard src/*.cpp)
