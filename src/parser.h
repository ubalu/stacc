#ifndef PARSER_H
#define PARSER_H

#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <ostream>

#include "tokenizer.h"

/// represents semantical data, used in both parsing and running
class Object {
public:
	using Type = enum {Int, Float, List, String, Block, Word};

	virtual Type type(void) const = 0;

	/// access the represented data
	virtual void* get_value(void) = 0;
	virtual const void* get_value(void) const = 0;

	/// needed for lists, `dup`, etc.
	virtual Object* clone(void) const = 0;

	virtual ~Object(void) {}
};

class OTInt: public Object {
	int64_t value;
public:
	OTInt(int64_t n = 0ll): value(n) {}
	
	Object::Type type(void) const override { return Object::Int; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	OTInt* clone(void) const override { return new OTInt(value); }
};

class OTFloat: public Object {
	double value;
public:
	OTFloat(double n = 0.0): value(n) {}
	
	Object::Type type(void) const override { return Object::Float; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	OTFloat* clone(void) const override { return new OTFloat(value); }
};

class OTWord: public Object {
	std::string* value;
public:
	OTWord(const char* s): value(new std::string(s)) {}
	OTWord(std::string const& s): value(new std::string(s)) {}
	
	Object::Type type(void) const override { return Object::Word; }

	void* get_value(void) override { return value; }
	const void* get_value(void) const override { return value; }

	OTWord* clone(void) const override { return new OTWord(*value); }

	~OTWord(void) { delete value; }
};

class OTBlock: public Object {
	std::vector<Object*>* value;
public:
	OTBlock(void): value(new std::vector<Object*>()) {}
	OTBlock(Object const& o): value(new std::vector<Object*>()) { value->push_back(o.clone()); }
	OTBlock(std::vector<Object*> const& v): value(new std::vector<Object*>()) {
		for (const Object* o: v) value->push_back(o->clone());
	}
	OTBlock(OTBlock const& b): value(new std::vector<Object*>()) {
		for (const Object* o: *(std::vector<Object*>*)b.get_value()) value->push_back(o->clone());
	}

	Object::Type type(void) const override { return Object::Block; }
	
	void* get_value(void) override { return value; }
	const void* get_value(void) const override {return value; }

	OTBlock* clone(void) const {
		OTBlock* result = new OTBlock();
		for (const Object* o: *value)
			((std::vector<Object*>*)result->get_value())->push_back(o->clone());
		return result;
	}
	~OTBlock(void) {
		for (Object* o: *value)
			delete o;
		delete value;
	}
};

class OTList: public Object {
	std::vector<Object*>* value; 
public:
	OTList(void): value(new std::vector<Object*>()) {}
	OTList(nullptr_t): value(nullptr) {}
	OTList(Object const& o): value(new std::vector<Object*>()) { value->push_back(o.clone()); } 
	OTList(std::vector<Object*> const& v): value (new std::vector<Object*>()) {
		for (const Object* o: v) value->push_back(o->clone());
	}
	OTList(OTList const& l): value(new std::vector<Object*>()) {
		for (const Object* o: *(std::vector<Object*>*)l.get_value()) value->push_back(o->clone());
	}
	
	Object::Type type(void) const override { return Object::List; }
	
	void* get_value(void) override { return value; }
	const void* get_value(void) const override { return value; }

	OTList* clone(void) const override {
		OTList* result = new OTList();
		for (const Object* o: *value)
			((std::vector<Object*>*)result->get_value())->push_back(o->clone());
		return result;
	}
	~OTList(void) {
		if (!value)
			return;
		for (Object* o: *value)
			delete o;
		delete value;
	}
};

class OTString: public Object {
	std::string* value;
public:
	OTString(void): value(new std::string()) {}
	OTString(std::string const& s): value(new std::string(s)) {}
	OTString(OTString const& s): value( new std::string(*(std::string*)s.get_value()) ) {}

	Object::Type type(void) const override { return Object::String; }

	void* get_value(void) override { return value; }
	const void* get_value(void) const override { return value; }

	OTString* clone(void) const override { return new OTString(*value); }

	~OTString(void) { delete value; }
};


using Tokens = std::vector<Token*>;
std::optional<std::vector<Object*>> parse(Tokens::const_iterator& begin, Tokens::const_iterator end, bool block = false, bool list = false);
std::ostream& operator<<(std::ostream& stream, Object const& o);

#endif
