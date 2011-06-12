
#pragma once

// Warning: If T's constructor throws, instance() will return a null reference.

template<class T, class N>
class Singleton : private boost::noncopyable
{

public:
	static T& instance()
	{
		boost::call_once(init, flag);
		return *t;
	}

	static void init() // never throws
	{
		t.reset(N::Alloc());
	}

protected:
	~Singleton() {}
	Singleton() {}

private:
	static boost::scoped_ptr<T> t;
	static boost::once_flag flag;
};

template<class T, class N> boost::scoped_ptr<T> Singleton<T, N>::t(0);
template<class T, class N> boost::once_flag Singleton<T, N>::flag = BOOST_ONCE_INIT;

