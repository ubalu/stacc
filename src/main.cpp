#include <iostream>
#include <fstream> 
#include <cstring>
#include <cerrno>
#include <optional>
#include <vector>

#include "tokenizer.h"

#define UNUSED(x) (void)(x);


#define ERROR "[\x1b[31mERROR\x1b[m] "

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cout << ERROR "Not implemented\n";
		return 1;
	}

	std::ifstream f{argv[1]};
	if (!f.is_open()) {
		std::cout << ERROR "File '" << argv[1] << "' could not be opened: " << strerror(errno) << "\n";
		return 1;
	}
	std::optional<std::vector<Token*>> tokens = tokenize(f);
	if (tokens) {
		for (Token* t: tokens.value()) 
			std::cout << *t << "\n";
		for (Token* t: tokens.value()) {
			put_value(std::cout, *t); std::cout << " ";
		}
		std::cout << "\n";
		for (Token* t: tokens.value())
			delete t;
	} else {
		std::cout << ERROR "Tokenization failed\n";
	}

	return 0;
}

