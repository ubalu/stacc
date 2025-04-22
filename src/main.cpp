#include <iostream>
#include <fstream> 
#include <cstring>
#include <cerrno>
#include <optional>
#include <vector>



#include "tokenizer.h"
#include "parser.h"
#include "defines.h"

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cout << ERROR "REPL mode not yet implemented\n";
		return 1;
	}

	std::cout << INFO "Loading '" << argv[1] << "'...\n";
	std::ifstream f{argv[1]};
	if (!f.is_open()) {
		std::cout << ERROR "File '" << argv[1] << "' could not be opened: " << strerror(errno) << "\n";
		return 1;
	}


	std::optional<std::vector<Token*>> m_tokens = tokenize(f);
	if (!m_tokens) {
		std::cout << ERROR "Tokenization failed\n";
		return 1;
	}
	std::vector<Token*> tokens = m_tokens.value();

	std::cout << INFO "TOKENS: \n";
	for (Token* t: tokens) {
		put_value(std::cout, *t); std::cout << " ";
	}
	std::cout << "\n";

	std::vector<Token*>::const_iterator begin = tokens.cbegin();
	std::optional<std::vector<Object*>> m_parsed = parse(begin, tokens.cend());

	if (!m_parsed) {
		std::cout << ERROR "Parsing failed\n";
		return 1;
	}
	std::vector<Object*> parsed = m_parsed.value();
	
	std::cout << INFO "PARSED OBJECTS:\n";
	for (Object* o: parsed)
		std::cout << *o << "\n";
	std::cout << "\n";

	for (Object* o: parsed)
		delete o;

	for (Token* t: tokens)
		delete t;

	return 0;
}

