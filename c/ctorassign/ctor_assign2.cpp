#include <iostream>
#include <cstdio>

using namespace std;

#define LOG_FUNC \
	printf("%s\n", __PRETTY_FUNCTION__);

#define LOG_FUNC2(type) \
	printf("%s %s\n", __PRETTY_FUNCTION__, type);

class A
{
public:
	// c
	A(std::string type = "default") : m_type(type)
	{
		LOG_FUNC2(m_type.c_str())
	}

	// copy
	A(const A &r) : m_type("copy")
	{
		LOG_FUNC2(m_type.c_str())
	}

 	// assign
	A& operator=(const A &r)
	{
		m_type = "assign";
		LOG_FUNC2(m_type.c_str())
	}

	~A()
	{
		LOG_FUNC2(m_type.c_str())
	}

private:
	std::string m_type;
};

A f(A a)
{
	LOG_FUNC
	return a;
}

const A& f2(const A &a)
{
	LOG_FUNC
	return a;
}

const A* f3(const A *a)
{
	LOG_FUNC
	return a;
}

main()
{
	A a("c");
	f(a);
	LOG_FUNC2("----------")
	f2(a);
	LOG_FUNC2("----------")
	f3(&a);
	return 0;
}
