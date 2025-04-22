#ifndef PARSER_H
#define PARSER_H

#include <cstdint>


/// represents semantical data, used in both parsing and running
class Object {
public:
	using Type = enum {Int, Float, List, Block, Word};

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
}

class OTFloat: public Object {
	double value;
public:
	OTFloat(double n = 0.0): value(n) {}
	
	Object::Type type(void) const override { return Object::Float; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	OTFloat* clone(void) const override { return new OTFloat(value); }
}

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
}

class OTList: public Object {
	std::vector<Object*>* value; 
public:
	OTList(void): value(new std::vector()) {}
	OTList(Object const& o): value(new std::vector()) { value.push_back(o.clone()); } 
	OTList(std::vector<Object*> const& v) {
		value = new std::vector();
		for (Object* o: v)
			value->push_back(o->clone());
	}
	OTList(OTList const& l) {
		value = new std::vector();
		for (const Object* o: l.get_data())
			value->push_back(o->clone());
	}

	Object::Type type(void) const override { return Object::List; }

	void* get_value(void) override { return value; }
	const void* get_value(void) const override { return value; }

	OTList* clone(void) const override {
		OTList* result = new OTList();
		for (Object* o: value)
			result.get_value()->push_back(o->clone());
		return result;
	}

	~OTList(void) {
		for (Object* o: value)
			delete o;
		delete value;
	}
	
}



#endif
