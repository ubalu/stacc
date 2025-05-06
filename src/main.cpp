/**
 * @file
 * @brief Wrapper a többi funkcionalitás körül.
 * @todo REPL mód
 * @todo Tesztek
 */
#include <iostream>
#include <fstream> 
#include <cstring>
#include <cerrno>
#include <optional>
#include <vector>

#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"

#define ERROR "[\x1b[91mERROR\x1b[m] "

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cout << ERROR "REPL mode not yet implemented\n";
		return 1;
	}

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

	/// @todo \c parse() rendberakása
	std::vector<Token*>::const_iterator begin = tokens.cbegin();
	std::optional<std::vector<Object*>> m_parsed = parse(begin, tokens.cend());

	if (!m_parsed) {
		std::cout << ERROR "Parsing failed\n";
		return 1;
	}
	std::vector<Object*> parsed = m_parsed.value();
	Error e = interpret(parsed);
	/// @todo Hiba helyének megjelölése
	switch (e) {
		case SUCCESS: break;
		case STACK_UNDERFLOW:
			/// @todo Várt elemek száma
			std::cout << ERROR "Stack underflow: not enough items in stack.\n";
			break;
		case TYPE_MISMATCH:
			/// @todo Várt és kapott típus
			std::cout << ERROR "Invalid operand types.\n";
			break;
		case NOT_IMPLEMENTED:
			// @todo Melyik szó nincs implementálva
			std::cout << ERROR "Not implemented.\n";
			break;
		case UNDEFINED_WORD:
			// @todo Melyik szó ismeretlen
			std::cout << ERROR "Undefined word.\n";
			break;
		case INCORRECT_VALUE:
			/// @todo Milyen értéket vártunk (valami szövegként összefoglalva)
			std::cout << ERROR "Incorrect value.\n";
			break;
	}

	for (Object* o: parsed)
		delete o;

	for (Token* t: tokens)
		delete t;

	return 0;
}

