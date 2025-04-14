#ifndef MAYBE_H
#define MAYBE_H


template <typename T>
class Maybe {
	union {char dummy, T data};
	bool engaged;
public:
	Maybe(void): dummy(0), engaged(false) {}

	Maybe(T const& t): data(t), engaged(true) {}

	~Maybe(void) { if (engaged) data.~T(); }

	void reset(void) { if (engaged) data.~T(); }
	
	T& value(void) { if (engaged) return data; }
	T const& value(void) const { if (engaged) return data; }

	template<class U> T value_or(U&& u) const { return engaged ? data : u; }

	template<class... Args> void emplace(Args&&... args) {
		data = T(&args...);
	}

	T operator*(void)

	bool has_value(void) const { return engaged; }
};


#endif
