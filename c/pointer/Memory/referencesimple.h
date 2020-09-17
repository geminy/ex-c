#ifndef REFERENCESIMPLE
#define REFERENCESIMPLE

#include <iostream>

class A
{
public:
    A()
        : m_refCount(1)
    {
        std::cout << "A::A" << std::endl;
    }

    A(const A &r)
        : m_refCount(1)
    {
        std::cout << "A::A copy-constructor" << std::endl;
    }

    ~A()
    {
        std::cout << "A::~A" << std::endl;
    }

    void attach()
    {
        std::cout << "A::attach" << std::endl;
        ++m_refCount;
    }

    void detach()
    {
        if (m_refCount != 0) {
            std::cout << "A::detach " << m_refCount << std::endl;
            if (--m_refCount == 0) {
                delete this;
            }
        }
    }

private:
    int m_refCount;
};

class B
{
public:
    B()
        : m_pA(new A)
    {
        std::cout << "B::B" << std::endl;
    }

    B(const B &r)
        : m_pA(r.m_pA)
    {
        std::cout << "B::B copy-constructor" << std::endl;
        m_pA->attach();
    }

    ~B()
    {
        std::cout << "B::~B" << std::endl;
        m_pA->detach();
    }

private:
    A* m_pA;
};

#endif // REFERENCESIMPLE

