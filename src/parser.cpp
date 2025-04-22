#include <optional>
#include <vector>
#include <iostream>
#include <cstdint>

#include "parser.h"
#include "tokenizer.h"
#include "defines.h"

std::optional<std::vector<Object*>>
parse(std::vector<Token*>::const_iterator& it, std::vector<Token*>::const_iterator end, bool block, bool list) {
	std::vector<Object*> result;
	for (; it != end; ++it) {
		switch ((*it)->type()) {
			case Token::Int:
				result.push_back(
					new OTInt( *(int64_t*)(*it)->get_value() )
				);
				break;
			case Token::Float:
				result.push_back(
					new OTFloat( *(double*)(*it)->get_value() )
				);
				break;
			case Token::String:
				result.push_back(
					new OTString( *(std::string*)(*it)->get_value() )
				);
				break;
			case Token::Word: {
				// we are parsing a block and we found the end:
				if (block && (**it == TTWord("]")))
					return std::optional<std::vector<Object*>>(result);

				// we are parsing a block, but we hit an incorrect terminator:
				else if (block && (**it == TTWord("}"))) {
					std::cout << ERROR "Interleaved block and list: found `}`, expected `]`\n";
					return std::nullopt;
				}

				// we are parsing a list and we hit the end:
				else if (list && (**it == TTWord("}"))) 
					return std::optional<std::vector<Object*>>(result);
				
				// we are parsing a list and we hit an incorrect terminator:
				else if (list && (**it == TTWord("]"))) {
					std::cout << ERROR "Interleaved list and block: found `]`, expected `}`\n";
					return std::nullopt;
				}
				
				// found block initializer word, recursively parse a block.
				else if (**it == TTWord("[")) {
					std::optional<std::vector<Object*>> blockdata = parse(++it, end, true, false);
					if (!blockdata) return std::nullopt;
					result.push_back( new OTBlock(blockdata.value()) );
					for (Object* o: blockdata.value())
						delete o;
				}

				// found list initializer word, recursively parse a list.
				else if (**it == TTWord("{")) {
					std::optional<std::vector<Object*>> listdata = parse(++it, end, false, true);
					if (!listdata) return std::nullopt;
					result.push_back( new OTList(listdata.value()) );
					for (Object* o: listdata.value())
						delete o;
				}

				else 
					result.push_back( new OTWord(*(std::string*)(*it)->get_value()) );
			} break;
		}
	}
	if (block) {
		std::cout << ERROR "Unterminated block\n";
		return std::nullopt;
	} else if (list) {
		std::cout << ERROR "Unterminated list\n";
		return std::nullopt;
	}
	return std::optional<std::vector<Object*>>(result);
}

std::ostream& operator<<(std::ostream& stream, Object const& o) {
	
	switch (o.type()) {
		case Object::Int:
			return stream << "Int(" << *(int64_t*)o.get_value() << ")";
		case Object::Float:
			return stream << "Float(" << *(double*)o.get_value() << ")";
		case Object::Word: 
			return stream << "Word(" << *(std::string*)o.get_value() << ")";
		case Object::String:
			return stream << "String(\"" << *(std::string*)o.get_value() << "\")";
		case Object::Block: {
			stream << "Block([";
			for (const Object* obj: *(std::vector<Object*>*)o.get_value())
				stream << *obj << ", ";
			return stream << "])";
		}
		case Object::List: {
			stream << "List({";
			for (const Object* obj: *(std::vector<Object*>*)o.get_value())
				stream << *obj << ", ";
			return stream << "})";
		}
 
	}

	return stream;
}


