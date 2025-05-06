
CXXFLAGS=-xc++ -Wall -Wextra -Wpedantic -Werror -std=c++17
DEBUGFLAGS=-g3 -ggdb
RELEASEFLAGS=-O3 -s

stacc: $(wildcard src/*.cpp src/*.hpp src/*.h) Makefile
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o staccd $(wildcard src/*.cpp)
	$(CXX) $(CXXFLAGS) $(RELEASEFLAGS) -o stacc $(wildcard src/*.cpp)

.PHONY: docs
docs: 
	doxygen 
	cd docs/doxygen/latex; \
		pdflatex refman; \
		makeindex refman.idx; \
		pdflatex refman; \
		pdflatex refman; \
		makeindex refman.idx; \
		pdflatex refman;\
	cd ../../..
	mv docs/doxygen/latex/refman.pdf . 
