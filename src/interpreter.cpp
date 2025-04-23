
#include <vector>
#include <unordered_map>
#include <iostream>

#include "interpreter.h"
#include "parser.h"

/**
 *	IMPLEMENTED BUILTINS (or the Stacc runtime):
 *  	Standard I/O:
 * 			[x] .			- basic print
 * 			[x] S.			- debug print
 * 			[ ] cr			- newline
 *
 *   	Arithmetic:
 *   		[ ] inc			- increment
 *   		[ ]	dec			- decrement
 *   		[x] +			- addition, string catenation
 *   		[ ] -			- subtraction
 *   		[ ] *			- multiplication
 *   		[ ] /			- division
 *   		[ ] %			- remainder (modulo)
 *   		[ ] pow			- power
 *   		[ ] sqrt		- square root
 *
 *   		[ ] deg2rad	 	- angle conversions
 *   		[ ] rad2deg		- angle conversions 
 *   		[ ] sin			- sine (radians)
 *   		[ ] cos			- cosine (radians)
 *   		[ ] tan 		- tangent (radians)
 *   		[ ] arcsin		- arcsine (radians)
 *   		[ ] arccos		- arccosine (radians)
 *   		[ ] arctan 		- arctangent (radians)
 *
 *   		[ ] and			- logical and
 *   		[ ] or			- logical or
 *   		[ ] not			- logical not
 *   		[ ] &			- bitwise and
 *   		[ ] |			- bitwise or
 *   		[ ] ^			- bitwise xor
 *   		[ ] ~			- bitwise not
 *
 *   	Comparison:
 *   		[ ] = 			- equal
 *   		[ ] !=			- not equal
 *   		[ ]	< 			- strictly less-than
 *   		[ ] <=			- less-than-or-equal
 *   		[ ] >			- strictly greater-than
 *   		[ ] >=			- greater-than-or-equal
 *
 *   	List operations:
 *   		[ ]	len			- length of list
 *   		[ ] append		- append item
 *   		[ ] prepend		- insert item at front
 *   		[ ] insert		- insert item after position
 *   		[ ] first		- get first item
 *   		[ ] last		- get last item
 *   		[ ] take		- first n items (if n is negative, last n items)
 * 
 *   		[ ] iota		- generate indices (numbers from 0 to n)
 *   		[ ] each		- apply function to each element (see 'curry')
 *			[ ] map			- transform each item in list
 *			[ ] filter		- remove items based on a predicate
 *			[ ] reduce		- apply function between elements (see Wiki: https://en.wikipedia.org/wiki/Fold_(higher-order_function))
 *			[ ] reduce1		- see 'reduce', use first element as starting value
 *			[ ] scan		- apply function between elements and save partial results (see https://en.wikipedia.org/wiki/Prefix_sum#Scan_higher_order_function)
 *			[ ] scan1		- see 'scan', use first element as starting value
 *		
 *		String operations: list operatiosńs work, and:
 *			TODO: expand this section
 *			[ ] upper		- change to uppercase
 *			[ ]	lower		- change to lowercase
 *			[ ] find		- find location of substring
 *			[ ] count		- count disjoint occurences of substring
 *
 * 		Stack operations:
 * 			[x] <literal>	- push <literal> on the stack
 * 			[x] dup			- duplicate top value
 * 			[ ]	2dup		- duplicate top pair
 * 			[ ]	swap		- switch top two elements
 * 			[ ]	2swap		- switch top two pairs
 * 			[ ] over		- duplicate second to top element to the top of the stack
 * 			[ ] 2over		- duplicate second to top pair to the top of the stack
 * 			[ ]	drop		- discard the top element
 * 			[ ] rot			- rotate the third-from-top element to the top of the stack
 *
 * 		Control flow:
 * 			[x] '[' and ']'	- block creation (this is a parse word)
 *			[ ] <word>		- call the builtin or defined word 'word'
 *			[ ] '<word>		- assign block to the name 'word'
 *			[ ]	if 			- conditionally execute blocks
 *			[ ]	curry		- partially apply value to block
 *
 * */
using namespace itp;
static const std::unordered_map<std::string, Word> builtin_words = {

	// TODO: implement all builtins
	
	/// drop-printing
	{".", [] (Stack& s) -> Error {
		if (s.size() < 1) 
			return STACK_UNDERFLOW;
		Object* o = s.back(); s.pop_back();
		std::cout << *o;
		delete o;
		return SUCCESS;
	}},

	/// debug printing
	{"S.", [] (Stack& s) -> Error {
		std::cout << "<" << s.size() << ">\n";
		for (auto o: s)
			std::cout << o << "\n";
		return SUCCESS;
	}},

	/// +: addition and string concatenation
	{"+", [] (Stack& s) -> Error {
		if (s.size() < 2)
			return STACK_UNDERFLOW;

		Object* top = s.back(); s.pop_back();
		switch (top->type()) {

			// int + ...
			case Object::Int: {
				int64_t val = *(int64_t*)top->get_value();
				Object* other = s.back(); s.pop_back();
				
				// int + int -> int
				if (other->type() == Object::Int) 
					s.push_back(new OTInt(
						val + *(int64_t*)other->get_value()
					));
				
				// int + float -> float
				else if (other->type() == Object::Float)
					s.push_back(new OTFloat(
						(double)val + *(double*)other->get_value()
					));

				// fail gracefully
				else { delete top; delete other; return TYPE_MISMATCH; }

				// prevent leaks
				delete top; delete other;
			} return SUCCESS;
			
			// float + ...
		 	case Object::Float: {
				double val = *(double*)top->get_value();
				Object* other = s.back(); s.pop_back();
				
				// float + int -> float
				if (other->type() == Object::Int)
					s.push_back(new OTFloat(
						val + (double)*(int64_t*)other->get_value()
					));

				// float + float -> float
				else if (other->type() == Object::Float)
					s.push_back(new OTFloat(
						val + *(double*)other->get_value()
					));
				
				// fail
				else { delete top; delete other; return TYPE_MISMATCH; }

				delete top; delete other;
			} return SUCCESS;

			// string + string
			case Object::String: {
				std::string const& val = *(std::string*)top->get_value();
				Object* other = s.back(); s.pop_back();
				if (other->type() == Object::String) {
					std::string const& val2 = *(std::string*)other->get_value();
					s.push_back(new OTString(
						val + val2
					));
					delete top; delete other;
				} else { delete top; delete other; return TYPE_MISMATCH; }
			} return SUCCESS;
			default:
				return TYPE_MISMATCH;
		}
	}},

	// dup: duplicating the item on top of the stack
	{"dup", [] (Stack& s) -> Error {
		if (s.size() < 1)
			return STACK_UNDERFLOW;
		Object* o = s.back();
		s.push_back(o->clone());
		return SUCCESS;
	}},
};

static std::unordered_map<std::string, Block> defined_words;

static Error (*execute_block)(Stack&, Block const&)  = [] (Stack& s, Block const& block) -> Error {
	Error e;
	for (Object* o: block) {
		switch (o->type()) {
			case Object::Block: case Object::Int: case Object::Float: case Object::String: case Object::List:
				s.push_back(o->clone());
				break;
			case Object::Word:
				std::string const& val = *(std::string*)o->get_value();
				// TODO: implement word definitions
				if (val.front() == '\'') {
					std::cout << "[\x1b[91mERROR\x1b[m] Assigning names is not yet implemented\n";
					return NOT_IMPLEMENTED;
				}

				e = (builtin_words.find(val) != builtin_words.end()) ? builtin_words.at(val)(s)
					: (defined_words.find(val) != defined_words.end()) ? execute_block(s, defined_words.at(val))
					: UNDEFINED_WORD;

				if (e != SUCCESS) return e;
				break;
		}
	}
	return SUCCESS;
};

Error interpret(std::vector<Object*> const& code) {
	Stack s;
	Error e = execute_block(s, code);
	for (Object* o: s)
		delete o;
	return e;
}

