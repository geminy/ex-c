#ifndef CAT
#define CAT

#include "animal.h"

class Cat : public Animal
{
public:
    Cat();
    ~Cat();

    void getName();
    void getColor();
};

#endif // CAT

