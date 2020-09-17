#include "animal.h"

Animal::Animal()
{
    std::cout << "Animal::Animal()" << std::endl;
}

Animal::~Animal()
{
    std::cout << "Animal::~Animal()" << std::endl;
}

void Animal::getKind()
{
    std::cout << "Animal::getKind()" << std::endl;
}

void Animal::getName()
{
    std::cout << "Animal::getName()" << std::endl;
}

void Animal::getColor()
{
    std::cout << "Animal::getColor()" << std::endl;
}
