#ifndef DOG
#define DOG

#include "animal.h"

class Dog : public Animal
{
public:
    Dog();
    ~Dog();

    void getName();
    void getColor();
};

#endif // DOG
