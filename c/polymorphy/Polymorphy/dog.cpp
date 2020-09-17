#include "dog.h"

Dog::Dog()
{
    std::cout << "Dog::Dog()" << std::endl;
}

Dog::~Dog()
{
    std::cout << "Dog::~Dog()" << std::endl;
}

void Dog::getName()
{
    std::cout << "Dog::getName()" << std::endl;
}

void Dog::getColor()
{
    std::cout << "Dog::getColor()" << std::endl;
}
