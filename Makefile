
CXXFLAGS=-xc++ -Wall -Wextra -Wpedantic -Werror -std=c++11

stacc: $(wildcard src/*.cpp src/*.hpp)
	$(CXX) $(CXXFLAGS) -o stacc $(wildcard src/*.cpp)
