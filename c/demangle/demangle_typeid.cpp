#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

enum MyEnum
{

};

union MyUnion
{

};

struct MyStruct
{

};

class MyClass
{
public:
    void memfunc();
    int memfunc2(int);
    int memfunc3(int, int);
};

void func();
int func2(int);
int func3(int, int);

namespace ns
{
enum MyEnum
{

};

union MyUnion
{

};

struct MyStruct
{

};

class MyClass
{
public:
    void memfunc();
    int memfunc2(int);
    int memfunc3(int, int);
};

void func();
int func2(int);
int func3(int, int);
}

int main(int argc, char *argv[])
{
    std::cout << typeid(char).name() << std::endl;
    std::cout << typeid(bool).name() << std::endl;
    std::cout << typeid(int).name() << std::endl;
    std::cout << typeid(float).name() << std::endl;
    std::cout << typeid(double).name() << std::endl;

    std::cout << typeid(int).name() << std::endl;
    std::cout << typeid(int&).name() << std::endl;
    std::cout << typeid(int*).name() << std::endl;

    std::cout << typeid(MyEnum).name() << std::endl;
    std::cout << typeid(MyUnion).name() << std::endl;
    std::cout << typeid(MyStruct).name() << std::endl;
    std::cout << typeid(MyClass).name() << std::endl;

    std::cout << typeid(func).name() << std::endl;
    std::cout << typeid(func2).name() << std::endl;
    std::cout << typeid(func3).name() << std::endl;

    MyClass c;
    std::cout << typeid(c.memfunc()).name() << std::endl;
    std::cout << typeid(c.memfunc2(0)).name() << std::endl;
    std::cout << typeid(c.memfunc3(0, 0)).name() << std::endl;

    std::cout << "--------------------" << std::endl;

    std::cout << typeid(ns::MyEnum).name() << std::endl;
    std::cout << typeid(ns::MyUnion).name() << std::endl;
    std::cout << typeid(ns::MyStruct).name() << std::endl;
    std::cout << typeid(ns::MyClass).name() << std::endl;

    std::cout << typeid(ns::func).name() << std::endl;
    std::cout << typeid(ns::func2).name() << std::endl;
    std::cout << typeid(ns::func3).name() << std::endl;

    ns::MyClass nsc;
    std::cout << typeid(nsc.memfunc()).name() << std::endl;
    std::cout << typeid(nsc.memfunc2(0)).name() << std::endl;
    std::cout << typeid(nsc.memfunc3(0, 0)).name() << std::endl;

    std::cout << "--------------------" << std::endl;

    std::cout << abi::__cxa_demangle(typeid(ns::MyClass).name(), 0, 0, 0) << std::endl;

    return 0;
}
