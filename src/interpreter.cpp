/**
 * @file 
 *
 *
 * @todo \c index és \c slice ? 
 * @todo \c sort helyett \c grade ?
 * @todo ezt bővíteni
 * @todo \c iota helyett \c [a..b], \c [0..b], \c [1..b] ?
 */

#include <vector>
#include <unordered_map>
#include <iostream>

#include "interpreter.h"
#include "parser.h"

/// Objektum értéke.
/** 
 * Ez a függvény gyakorlatilag egy makró, de így típusbiztos.
 * @warning A függvény nem ellenőrzi, hogy megfelelő típust kér-e!
 * @tparam T Az objektum \c value mezejének típusa.
 * @param o Az objektum, aminek az értékét keressük.
 * @returns Az objektum értéke, megfelelő típusra konvertálva.
 */
template<typename T> static inline T& value(Object* o) {
	return *(T*)o->get_value();
}

/// Verem legfelső elemét levenni (\c pop).
/**
 * Ez a függvény is lehetne makró, csak így típusbiztos. Igazából csak azt nem értem, 
 * hogy a beépített std::vector<T>::pop_back() miért nem ezt csinálja...
 * @warning A függvény nem ellenőrzi, hogy _van-e_ egyáltalán érték a veremben!
 * @param stack A verem, amiről leveszünk.
 * @returns Az adott verem utolsó eleme.
 */
static inline Object* pop(Stack& stack) {
	Object* p = stack.back();
	stack.pop_back();
	return p;
}

/** @def WORD_HEADER
 *  @brief Beépített szavak függvényfejléce.
 */
#define WORD_HEADER [] (Environment& env) -> Error

/** @def UNUSED(arg)
 *  @brief Nem használt argumentum.
 */
#define UNUSED(arg) (void)((arg));

static Error execute_block(Environment&, Block const&);

/// @var static const std::unordered_map<std::string, Word> builtin_words
/// @todo Implement all built-ins.
static const std::unordered_map<std::string, Word> builtin_words = {
// STANDARD I/O	
	// drop-printing
	{".", WORD_HEADER {
		if (env.stack.size() < 1) return STACK_UNDERFLOW;
		Object* o = pop(env.stack);
		std::cout << *o;
		delete o;
		return SUCCESS;
	}},

	// debug printing
	{"S.", WORD_HEADER {
		std::cout << "\n<" << env.stack.size() << ">\n";
		for (auto o: env.stack)
			std::cout << *o << "\n";
		return SUCCESS;
	}},

	// insert newline into stdout
	{"cr", WORD_HEADER {
		UNUSED(env)
		std::cout << "\n";
		return SUCCESS;
	}},

// ARITHMETIC
	// add 1 to a number
	{"inc", WORD_HEADER {
		if (env.stack.size() < 1) return STACK_UNDERFLOW;
		Object* o = pop(env.stack);
		switch (o->type()) {
			case Object::Int:
				env.stack.push_back(new OTInt( value<int64_t>(o) + 1 ));
				delete o;
				return SUCCESS;
			case Object::Float:
				env.stack.push_back(new OTFloat( value<double>(o) + 1.0 ));
				delete o;
				return SUCCESS;
			default:
				delete o;
				return TYPE_MISMATCH;
		}
	}},

	// subtract 1 from a number
	{"dec", WORD_HEADER {
		if (env.stack.size() < 1) return STACK_UNDERFLOW;
		Object* o = pop(env.stack);
		switch (o->type()) {
			case Object::Int:
				env.stack.push_back(new OTInt( value<int64_t>(o) - 1 ));
				delete o;
				return SUCCESS;
			case Object::Float:
				env.stack.push_back(new OTFloat( value<double>(o) - 1.0));
				delete o;
				return SUCCESS;
			default:
				delete o;
				return TYPE_MISMATCH;
		}
	}},
	
	// +: addition and string concatenation
	{"+", WORD_HEADER {
		if (env.stack.size() < 2) return STACK_UNDERFLOW;
		Object* top		= pop(env.stack); Object::Type tt = top->type();
		Object* bottom	= pop(env.stack); Object::Type bt = bottom->type();
		switch (tt << 8 | bt) {
			// Int, Int
			case 0x00: {
				int64_t tv = value<int64_t>(top);
				int64_t bv = value<int64_t>(bottom);
				env.stack.push_back(new OTInt( bv + tv ));
			} break;
			// Int, Float
			case 0x01: { 
				double tv = (double)value<int64_t>(top);
				double bv = value<double>(bottom);
				env.stack.push_back(new OTFloat( bv + tv ));
			} break;
			// Float, Int
			case 0x10: {
				double tv = value<double>(top);
				double bv = (double)value<int64_t>(bottom);
				env.stack.push_back(new OTFloat( bv + tv ));
			} break;
			// Float, Float
			case 0x11: {
				double tv = value<double>(top);
				double bv = value<double>(bottom);
				env.stack.push_back(new OTFloat( bv + tv ));
			} break;
			// String, String
			case 0x33: {
				std::string const& tv = value<std::string>(top);
				std::string const& bv = value<std::string>(bottom);
				env.stack.push_back(new OTString( bv + tv ));
			} break; 
			default:
				delete top; delete bottom;
				return TYPE_MISMATCH;
		}
		delete top; delete bottom;
		return SUCCESS;
	}},
	
	// *: multiplication
	{"*", WORD_HEADER {
		if (env.stack.size() < 2) return STACK_UNDERFLOW;
		Object* top		= pop(env.stack); Object::Type tt = top->type();
		Object* bottom	= pop(env.stack); Object::Type bt = bottom->type();
		switch (tt << 8 | bt) {
			// Int, Int
			case 0x00: {
				int64_t tv = value<int64_t>(top);
				int64_t bv = value<int64_t>(bottom);
				env.stack.push_back(new OTInt( bv * tv ));
			} break;
			// Int, Float
			case 0x01: { 
				double tv = (double)value<int64_t>(top);
				double bv = value<double>(bottom);
				env.stack.push_back(new OTFloat( bv * tv ));
			} break;
			// Float, Int
			case 0x10: {
				double tv = value<double>(top);
				double bv = (double)value<int64_t>(bottom);
				env.stack.push_back(new OTFloat( bv * tv ));
			} break;
			// Float, Float
			case 0x11: {
				double tv = value<double>(top);
				double bv = value<double>(bottom);
				env.stack.push_back(new OTFloat( bv * tv ));
			} break;
			default:
				delete top; delete bottom;
				return TYPE_MISMATCH;
		}
		delete top; delete bottom;
		return SUCCESS;
	}},

// COMPARISONS
	// <: strictly less-than
	// TODO: consider supporting strings
	{"<", WORD_HEADER {
		if (env.stack.size() < 2) return STACK_UNDERFLOW;
		Object* top		= pop(env.stack); Object::Type tt = top->type();
		Object* bottom	= pop(env.stack); Object::Type bt = bottom->type();
		switch (tt << 8 | bt) {
			// Int, Int
			case 0x00: {
				int64_t tv = value<int64_t>(top);
				int64_t bv = value<int64_t>(bottom);
				env.stack.push_back(new OTInt( (int64_t)(bv < tv) ));
			} break;
			// Int, Float
			case 0x01: { 
				double tv = (double)value<int64_t>(top);
				double bv = value<double>(bottom);
				env.stack.push_back(new OTInt( (int64_t)(bv < tv) ));
			} break;
			// Float, Int
			case 0x10: {
				double tv = value<double>(top);
				double bv = (double)value<int64_t>(bottom);
				env.stack.push_back(new OTInt( (int64_t)(bv < tv) ));
			} break;
			// Float, Float
			case 0x11: {
				double tv = value<double>(top);
				double bv = value<double>(bottom);
				env.stack.push_back(new OTInt( (int64_t)(bv < tv) ));
			} break;
			default:
				return TYPE_MISMATCH;
		}
		delete top; delete bottom;
		return SUCCESS;
	}},

// LIST OPERATIONS
	// iota: index generator, create a list from 0 to n-1
	// negative value is an error.
	// TODO: negative input as reverse list
	{"iota", WORD_HEADER {
		if (env.stack.size() < 1) return STACK_UNDERFLOW;
		Object* top = pop(env.stack);
		if (top->type() == Object::Int) {
			int64_t end = value<int64_t>(top);
			if (end < 0) return INCORRECT_VALUE;
			env.stack.push_back(new OTList());
			std::vector<Object*>& list_val = value<std::vector<Object*>>(env.stack.back());
			for (int64_t i = 0; i < end; i++)
				list_val.push_back(new OTInt(i));
			delete top;
			return SUCCESS;
		}
		delete top;
		return TYPE_MISMATCH;
	}},

	// map: transform list by applying function
	/// @bug Memóriahibás (valószínűleg ez a függvény)
	{"map", WORD_HEADER {
		Object* fn = pop(env.stack);
		Object* list = env.stack.back(); // simpler to modify in-place
		if (fn->type() == Object::Block && list->type() == Object::List) {
			std::vector<Object*> const& fn_body = value<std::vector<Object*>>(fn);
			std::vector<Object*>& items = value<std::vector<Object*>>(list);
			Stack s; std::unordered_map<std::string, Block> w;
			Environment tmp_env{s, w};
			for (size_t idx = 0; idx < items.size(); idx++) {
				for (auto i: tmp_env.stack)
					delete i;
				auto item = items.at(idx);
				tmp_env.stack.push_back(item);
				Error e = execute_block(tmp_env, fn_body);
				if (e != SUCCESS) {
					delete fn; delete list;
					for (auto i: tmp_env.stack)
						delete i;
					return e;
				}
				delete item;
				item = tmp_env.stack.back()->clone();

			}
		}
		delete fn;
		return TYPE_MISMATCH;
	}},

	// reduce1: apply function between each element, using the first element as initial value
	{"reduce1", WORD_HEADER {
		if (env.stack.size() < 2) return STACK_UNDERFLOW;
		Object* fn = pop(env.stack);
		Object* list = pop(env.stack);
		if (fn->type() == Object::Block && list->type() == Object::List) {
			std::vector<Object*> const& fn_body = value<std::vector<Object*>>(fn);
			std::vector<Object*> const& val = value<std::vector<Object*>>(list);
			
			if (val.size() == 0) {
				delete fn; delete list;
				return SUCCESS;
			} else if (val.size() == 1) {
				env.stack.push_back(val.at(0)->clone());
				delete fn; delete list;
			}

			env.stack.push_back(val.at(0)->clone());
			for (auto it = val.cbegin() + 1; it != val.cend(); ++it) {
				env.stack.push_back((*it)->clone());
				Error e = execute_block(env, fn_body);
				if (e != SUCCESS) {
					delete fn; delete list;
					return e;
				}
			}

			delete fn; delete list;
			return SUCCESS;
		}
		delete fn; delete list;
		return TYPE_MISMATCH;

	}},
// STRING OPERATIONS

// STACK OPERATIONS
	// dup: duplicating the item on top of the stack
	{"dup", WORD_HEADER {
		if (env.stack.size() < 1) return STACK_UNDERFLOW;
		Object* o = env.stack.back();
		env.stack.push_back(o->clone());
		return SUCCESS;
	}},

	// drop: discard top item of stack
	{"drop", WORD_HEADER {
		if (env.stack.size() < 1) return STACK_UNDERFLOW;
		Object* o = pop(env.stack);
		delete o; 
		return SUCCESS;
	}},

// CONTROL FLOW
	// if: conditionally select next block
	{"if", WORD_HEADER {
		if (env.stack.size() < 3) return STACK_UNDERFLOW;
		Object* if_false	= pop(env.stack); 
		Object* if_true		= pop(env.stack); 
		Object* predicate	= pop(env.stack); 
		if (predicate->type() == Object::Int
			&& if_true->type() == Object::Block
			&& if_false->type() == Object::Block) {
			Error e = execute_block(env, value<std::vector<Object*>>(value<int64_t>(predicate) ? if_true : if_false));
			delete predicate; delete if_true; delete if_false;
			return e;
		} else {
			delete predicate; delete if_true; delete if_false;
			return TYPE_MISMATCH;
		}
	}},
};

/// Egy blokk futtatása.
/**
 *	Lefuttat egy blokkot, azaz sorrendben mindegyik elemre végrehajtja a 
 *	megfelelő utasítást.
 *	@param env A futtatási környezet.
 *	@param block A lefuttatandó blokk.
 *	@returns A futtatásból származó hiba.
 */
static Error execute_block(Environment& env, Block const& block) {
	Error e;
	for (Object* o: block) {
		switch (o->type()) {
			case Object::Block: case Object::Int: case Object::Float: case Object::String: case Object::List:
				env.stack.push_back(o->clone());
				break;
			case Object::Word:
				std::string val = std::string(*(std::string*)o->get_value());
				if (val.front() == '\'') {
					if (env.stack.size() < 1) return STACK_UNDERFLOW;
					Object* o2 = pop(env.stack);
					if (o2->type() != Object::Block)
						return TYPE_MISMATCH;

					std::string new_word = val.substr(1);
					env.defined_words[new_word] = std::vector<Object*>();
					for (Object* obj: *(std::vector<Object*>*)o2->get_value()) {
						env.defined_words[new_word].push_back(obj->clone());
						delete obj;
					}
				} else {
					e = (builtin_words.find(val) != builtin_words.end()) ? builtin_words.at(val)(env)
						: (env.defined_words.find(val) != env.defined_words.end()) ? execute_block(env, env.defined_words.at(val))
						: UNDEFINED_WORD;

					if (e != SUCCESS) {
						std::cout << "Running word " << val << "\n";
						return e;
					}
				}
				break;
		}
	}
	return SUCCESS;
};

Error interpret(std::vector<Object*> const& code) {
	Stack s; std::unordered_map<std::string, Block> w;
	Environment env{s, w};
	Error e = execute_block(env, code);
	for (Object* o: s)
		delete o;
	return e;
}

