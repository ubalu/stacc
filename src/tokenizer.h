#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <istream>

/// This class represents a token (a group of characters 
/// with associated meaning).
///
/// It is implemented as a kind of glorified tagged union.
class Token {
public:
	/// types of literals
	using Type = enum {Int, Float, String, Word}; 

	/// tell the type of the token 
	virtual Type type(void) const = 0;
	
	/// get the value the token represents
	virtual void* get_value(void) = 0;
	virtual const void* get_value(void) const = 0;

	/// helps with parsing
	virtual bool operator==(Token const& other) const = 0;
	
	/// some tokens may need heap memory
	virtual ~Token() {}
};

class TTInt: public Token {
	int64_t value;
public:
	TTInt(int64_t n = 0ll): value(n) {}
	
	Token::Type type(void) const override { return Token::Int; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	bool operator==(Token const& other) const { return other.type() == Token::Int && *(int64_t*)other.get_value() == value; }
};

class TTFloat: public Token {
	double value;
public:
	TTFloat(double x = 0.0): value(x) {}
	
	Token::Type type(void) const override { return Token::Float; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	bool operator==(Token const& other) const { return other.type() == Token::Float && *(double*)other.get_value() == value; }
};

class TTString: public Token {
	std::string* value; 
public:
	TTString(void): value(new std::string("")) {}
	TTString(std::string const& str): value(new std::string(str)) {}
	TTString(const char* s): value(new std::string(s)) {}

	Token::Type type(void) const override { return Token::String; }

	void* get_value(void) override { return value; }
	const void* get_value(void) const override { return value; }

	~TTString(void) { delete value; }

	bool operator==(Token const& other) const { return other.type() == Token::String && *(std::string*)other.get_value() == *value; }
};


class TTWord: public Token {
	std::string* value; 
public:
	TTWord(std::string const& str): value(new std::string(str)) {}
	TTWord(const char* s): value(new std::string(s)) {}

	Token::Type type(void) const override { return Token::Word; }

	void* get_value(void) override { return value; }
	const void* get_value(void) const override { return value; }

	~TTWord(void) { delete value; }

	bool operator==(Token const& other) const { return other.type() == Token::Word && *(std::string*)other.get_value() == *value; }
};

std::optional<std::vector<Token*>> tokenize(std::istream& stream);

void put_value(std::ostream& stream, Token const& t);
std::ostream& operator<<(std::ostream& stream, Token const& t);

#endif
