/**
 * @file
 * @brief  Program futtatásához használt típusok.
 */
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <unordered_map>

#include "parser.h"

/// @{
/// Szemantikai sugallatú alias-ok.
using Stack = std::vector<Object*>;
using Block = std::vector<Object*>;
/// @}

/// Program futtatásának környezete.
struct Environment {
	Stack& stack;
	std::unordered_map<std::string, Block>& defined_words;
};

/// Futás közben előforduló hibák.
enum Error {
	SUCCESS = 0,
	STACK_UNDERFLOW,
	TYPE_MISMATCH,
	NOT_IMPLEMENTED,
	UNDEFINED_WORD,
	INCORRECT_VALUE,
};

/// Beépített szavakat futtató függvények típusa.
using Word = Error (*)( Environment& ); 

/// Program futtatása.
/**
 * Szintaktikailag analizált program lefuttatása. Kezeli a futó program környezetét,
 * és a felmerülő hibákat.
 * @param code A futtatandó objektumok listája.
 * @returns A futtatott program hibaüzenete.
 */
Error interpret(std::vector<Object*> const& code);

#endif

