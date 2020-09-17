#ifndef COMMON
#define COMMON

#include <iostream>

class Common
{
public:
    Common()
    {
        std::cout << "Common::Common" << std::endl;
    }

    Common(const Common &r)
    {
        std::cout << "Common::Common copy-constructor" << std::endl;
    }

    ~Common()
    {
        std::cout << "Common::~Common" << std::endl;
    }
};

#endif // COMMON

