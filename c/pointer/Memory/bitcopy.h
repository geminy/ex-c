#ifndef BITCOPY
#define BITCOPY

#include "common.h"

class BitCopy
{
public:
    BitCopy()
        : m_p(new Common)
    {
        std::cout << "BitCopy::BitCopy" << std::endl;
    }

    ~BitCopy()
    {
        std::cout << "BitCopy::~BitCopy" << std::endl;
        if (m_p) {
            delete m_p;
            m_p = NULL;
        }
    }

private:
    Common *m_p;
};

#endif // BITCOPY

