/**
 * @file 
 * @brief Osztály- és függvénydeklarációk tokenizáláshoz.
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <istream>

/// Karaktercsoportok extra jelentéssel.
class Token {
public:
	/// A várható típusok.
	enum Type {
		/// Egész literál.
		Int,
		/// Valós literál.
		Float,
		/// Szöveg literál.
		String,
		/// Más szó.
		Word
	}; 

	/// A token típusának lekérdezése.
	/// @returns A token típusa.
	virtual Type type(void) const = 0;
	
	/// Token értékének lekérdezése.
	/// @returns Pointer a token értékére.
	virtual void* get_value(void) = 0;
	/// @overload
	virtual const void* get_value(void) const = 0;

	/// Segítő függvény szintaktikai analízishez.
	/// @param other A token, amihez hasonlítunk.
	/// @returns Egyenlő-e a két token.
	virtual bool operator==(Token const& other) const = 0;
	
	virtual ~Token() {}
};

/// Szám literál.
/**
 *  Egy <a href="https://en.cppreference.com/w/cpp/string/basic_string/stol">C-stílusú</a> 64-bites, előjeles egész literál. 
 */
class TTInt: public Token {
	int64_t value;
public:
	TTInt(int64_t n = 0ll): value(n) {}
	
	Token::Type type(void) const override { return Token::Int; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	bool operator==(Token const& other) const override { return other.type() == Token::Int && *(int64_t*)other.get_value() == value; }
};

/// Valós literál.
/**
 * Egy [C-stílusú](https://en.cppreference.com/w/cpp/string/basic_string/stof) dupla pontosságú lebegőpontos szám literál.
 */
class TTFloat: public Token {
	double value;
public:
	TTFloat(double x = 0.0): value(x) {}
	
	Token::Type type(void) const override { return Token::Float; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	bool operator==(Token const& other) const override { return other.type() == Token::Float && *(double*)other.get_value() == value; }
};


/// String literál.
/**
 * Egy szöveges literál, \c " -től \c " -ig tart. 
 * @todo Escape karakterek.
 */
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

	bool operator==(Token const& other) const override { return other.type() == Token::String && *(std::string*)other.get_value() == *value; }
};

/// Egyszerű szó
/**
 * Más, különleges tokenként nem értelmezett szó.
 */
class TTWord: public Token {
	std::string* value; 
public:
	TTWord(std::string const& str): value(new std::string(str)) {}
	TTWord(const char* s): value(new std::string(s)) {}

	Token::Type type(void) const override { return Token::Word; }

	void* get_value(void) override { return value; }
	const void* get_value(void) const override { return value; }

	~TTWord(void) { delete value; }

	bool operator==(Token const& other) const override { return other.type() == Token::Word && *(std::string*)other.get_value() == *value; }
};

/// Stream tokenizálasa.
/**
 * @param stream A bemeneti stream, ahonnan a forrást olvassuk.
 * @returns Siker esetén a tokenek listáját. A tárolt tokeneket a hívó birtokolja. 
 * 			Hiba esetén  \code{.cpp} std::nullopt \endcode.
 */
std::optional<std::vector<Token*>> tokenize(std::istream& stream);

/// @deprecated Valószínűleg többet nem fogom használni, a végső beadás előtt
/// 			valószínűleg kikerül a codebase-ből.
void put_value(std::ostream& stream, Token const& t);
/// @deprecated Valószínűleg többet nem fogom használni, a végső beadás előtt
/// 			valószínűleg kikerül a codebase-ből.
std::ostream& operator<<(std::ostream& stream, Token const& t);


#endif
