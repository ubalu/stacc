/**
 * @file
 * @brief Tokenizálás implementáció.
 * @todo A kiíró függvényeket elhagyni
 */
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdint>

#include "tokenizer.h"

#define ERROR "[\x1b[91mERROR\x1b[m] "

std::optional<std::vector<Token*>> tokenize(std::istream& stream) {
	std::vector<Token*> tokens;

	// while the stream is not empty
	while (stream.peek() != EOF) {	

		// skip whitespace
		while ( isspace(stream.peek()) ) (void)stream.get();
		if (stream.peek() == EOF) break;

		// handle strings
		/// @todo handle escape strings
		if (stream.peek() == '"') {
			
			// discard introducing `"`
			(void)stream.get();
			std::string acc;
			
			// get characters until next `"`
			while (stream.peek() != '"') {

				// this is an error
				if (stream.peek() == EOF) {
					std::cout << ERROR "Unterminated string literal";
					return std::nullopt;
				}

				// save characters
				acc.push_back(stream.get());
			}

			// discard the final `"`
			(void)stream.get();

			// save as a string literal
			tokens.push_back(new TTString(acc));
		} else {
			// get a whitespace-delimited word
			std::string word;
			stream >> word;

			// discard comments
			if (word == "!") {
				// skip all characters until newline
				while (stream.peek() != '\n') {
					if (stream.peek() == EOF)
						// if at the end of the stream, just return all the tokens
						return std::optional<std::vector<Token*>>(tokens);
					(void)stream.get();
				}
			// otherwise 
			} else {
				// try to parse `word` as an int or a double
				// using built-in functions really helps with the parsing (e.g. scientific notation, `0x` notation etc.)
				// basically, all c-style numbers are supported
				size_t read_int, read_dbl;
				try {
					int64_t n = stoll(word, &read_int, 0); double d = stod(word, &read_dbl);
					if (read_int == word.size())
						tokens.push_back(new TTInt(n));
					else if (read_dbl == word.size())
						tokens.push_back(new TTFloat(d));
					else 
						tokens.push_back(new TTWord(word));
				// stoll and stod throw if no characters can be parsed -> `word` is a word literal
				/// @todo Máshogy megoldani? Exception-ök szinte úgy fájnak, mint Trianon
				} catch(std::invalid_argument const&) {
					tokens.push_back(new TTWord(word));
				}
			}
		}
	}
	return std::optional<std::vector<Token*>>(tokens);
}

void put_value(std::ostream& stream, Token const& t) {
	switch (t.type()) {
		case Token::Int:
			stream << *(int64_t*)t.get_value();
			return;
		case Token::Float:
			 stream << *(double*)t.get_value();
			return;
		case Token::String:
			 stream << "\"" << *(std::string*)t.get_value() << "\"";
			return;
		case Token::Word:
			 stream << *(std::string*)t.get_value();
			return;
		default: {
			std::cout << ERROR "Unreachable";
			return; 
		}
	}
}

std::ostream& operator<<(std::ostream& stream, Token const& t) {
	switch (t.type()) {
		case Token::Int:
			return stream << "INT(" << *(int64_t*)t.get_value() << ")";
		case Token::Float:
			return stream << "FLOAT(" << *(double*)t.get_value() << ")";
		case Token::String:
			return stream << "STRING(\"" << *(std::string*)t.get_value() << "\")";
		case Token::Word:
			return stream << "WORD(" << *(std::string*)t.get_value() << ")";
		default: {
			std::cout << ERROR "Unreachable";
			return stream;
		}
	}
}


