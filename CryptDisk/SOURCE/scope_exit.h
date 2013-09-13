// Boost independent version of SCOPE_EXIT.

#pragma once

#include <functional>

class scope_exit_t
{
	typedef std::function<void()> func_t;

public:
	scope_exit_t(func_t &&f) : func(f) {}
	~scope_exit_t() { func(); }

private:
	// Prohibit construction from lvalues.
	scope_exit_t(func_t &);

	// Prohibit copying.
	scope_exit_t(const scope_exit_t&);
	const scope_exit_t &operator=(const scope_exit_t &);

	// Prohibit new/delete.
	void *operator new(size_t);
	void *operator new[](size_t);
	void operator delete(void *);
	void operator delete[](void *);

	const func_t func;
};

#define SCOPE_EXIT_CAT2(x, y) x##y
#define SCOPE_EXIT_CAT1(x, y) SCOPE_EXIT_CAT2(x, y)
#define SCOPE_EXIT scope_exit_t SCOPE_EXIT_CAT1(scope_exit_, __COUNTER__) = [&]
