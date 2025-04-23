#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>

#include "parser.h"
namespace itp {
	using Stack = std::vector<Object*>;
	using Block = std::vector<Object*>;
	enum Error {
		SUCCESS = 0,
		STACK_UNDERFLOW,
		TYPE_MISMATCH,
		NOT_IMPLEMENTED,
		UNDEFINED_WORD,
	}; 
	using Word = auto (*)( Stack& ) -> Error; 
}
itp::Error interpret(std::vector<Object*> const& code);

#endif

