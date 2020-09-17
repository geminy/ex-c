#include <iostream>
#include <cstdio>

using namespace std;

#define LOG_FUNC(name, origin) \
	printf("%s %d %s\n", __PRETTY_FUNCTION__, name, origin);

class A
{
public:
	// c
	A(int name) : m_name(name), m_origin("c")
	{
		LOG_FUNC(m_name, m_origin.c_str())
	}

	// copy
	A(const A &r) : m_name(r.m_name), m_origin("copy")
	{
		LOG_FUNC(m_name, m_origin.c_str())
	}

 	// assign
	A& operator=(const A &r)
	{
		m_name = r.m_name;
		m_origin = "assign";
		LOG_FUNC(m_name, m_origin.c_str())
	}

	~A()
	{
		LOG_FUNC(m_name, m_origin.c_str())
	}

private:
	int m_name;
	string m_origin;
};

main()
{
	//! case 5/6
	A a(0);

	// 1 c
	/*
	A a1(1);*/

	// 2 c
	/*
	A a2 = A(2);*/

	
	// 3 copy
	/*
	A a3 = a;*/

	// 4 copy
	/*
	A a4 = A(a);*/

	// 5 assign
	/*
	A a5(5);
	a5 = a;*/

	// 6 copy/assign
	/*
	A a6(6);
	a6 = A(a);*/

	return 0;
}
