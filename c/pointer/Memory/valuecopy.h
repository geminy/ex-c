#ifndef VALUECOPY
#define VALUECOPY

#include "common.h"

class ValueCopy
{
public:
    ValueCopy()
        : m_p(new Common)
    {
        std::cout << "ValueCopy::ValueCopy" << std::endl;
    }

    ValueCopy(const ValueCopy &r)
        : m_p(new Common(*r.m_p))
    {
        std::cout << "ValueCopy::ValueCopy copy-constructor" << std::endl;
    }

    ~ValueCopy()
    {
        std::cout << "ValueCopy::~ValueCopy" << std::endl;
        if (m_p) {
            delete m_p;
            m_p = NULL;
        }
    }

private:
    Common *m_p;
};

#endif // VALUECOPY

