#ifndef ANIMAL
#define ANIMAL

#include <iostream>

class Animal
{
public:
    Animal();
    virtual ~Animal();

    void getKind();
    void getName();
    virtual void getColor();
};

#endif // ANIMAL
