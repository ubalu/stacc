/**
 * @file
 * @brief Szintaktikai analízishez és futtatáshoz szükséges osztályok.
 */
#ifndef PARSER_H
#define PARSER_H

#include <cstdint>
#include <vector>
#include <string>
#include <optional>
#include <ostream>

#include "tokenizer.h"

/// Program futása közben használt objektum.
/**
 *	Tokenek analizálásakor is használt alaposztály, a szintaxis szinte nemléte
 *	miatt a program futásakor is kiválóan használható osztály. (Más nyelvekben
 *	ebben a lépésben expression, statement stb. analízis kellene, itt a nyelv
 *	struktúrája miatt ezzel nem kell foglalkoznunk.)
 */
class Object {
public:
	/// Objektumok lehetséges típusai.
	/**
	 * Mivel minden objektum ilyen típusú kell legyen, ezek a nyelv adattípusai 
	 * is. 
	 * Az, hogy \c uint8_t típusú, a szavak típusellenőrzését 
	 * könnyíti meg (több objektum esetén is elég egy switch statement).
	 */
	enum Type: uint8_t {
		/// Egész
		Int = 0x00,
		/// Valós
		Float = 0x01,
		/// Lista
		List = 0x02,
		/// Szöveg
		String = 0x03,
		/// Blokk
		Block = 0x04,
		/// Egyéb szó
		Word = 0x05
	};

	/// Objektum típusának lekérdezése.
	/// @returns Az objektum típusa.
	virtual Type type(void) const = 0;

	/// Objektum értékének lekérdezése.
	/// @returns Pointer az objektum értékére.
	virtual void* get_value(void) = 0;
	/// @overload
	virtual const void* get_value(void) const = 0;


	/// Mély másolat készítése.
	/**
	 * Mély másolatot készít önmagáról, pl. lista az elemeit is klónozza.
	 * @todo Ne abuzáljam ennyire.
	 * @returns Pointer egy új objektumra, amelyet a hívó birtokol.
	 */
	virtual Object* clone(void) const = 0;

	virtual ~Object(void) {}
};

/// Egész érték.
class OTInt: public Object {
	int64_t value;
public:
	OTInt(int64_t n = 0ll): value(n) {}
	
	Object::Type type(void) const override { return Object::Int; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	OTInt* clone(void) const override { return new OTInt(value); }
};

/// Valós érték.
class OTFloat: public Object {
	double value;
public:
	OTFloat(double n = 0.0): value(n) {}
	
	Object::Type type(void) const override { return Object::Float; }

	void* get_value(void) override { return &value; }
	const void* get_value(void) const override { return &value; }

	OTFloat* clone(void) const override { return new OTFloat(value); }
};

/// Meghívható (vagy definiálandó) szó.
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

/// Olyan objektum-kollekció, amit le tudunk futtatni.
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

	OTBlock* clone(void) const override {
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

/// Rendezett heterogén gyűjtemény.
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

/// Karakterlánc.
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

/// Szintaktikai analízist végez tokenizált programon.
/**
 * @param begin A tokenlista elejére mutató iterátor.
 * @param end	A tokenlista vége után mutató iterátor.
 * ☺param block Jelzi, hogy blokkot várunk-e.
 * @param list	Jelzi, hogy listát várunk-e.
 * @todo Kezelje az iterátorokat magának -> legyen szétszedve két függvényre.
 */
std::optional<std::vector<Object*>> parse(
	std::vector<Token*>::const_iterator& begin,
	std::vector<Token*>::const_iterator end,
	bool block = false,
	bool list = false
);

/// Inserter
/// @deprecated Valószínűleg további haszna nincs.
std::ostream& operator<<(std::ostream& stream, Object const& o);

#endif
