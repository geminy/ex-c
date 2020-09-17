#include "cat.h"

Cat::Cat()
{
    std::cout << "Cat::Cat()" << std::endl;
}

Cat::~Cat()
{
    std::cout << "Cat::~Cat()" << std::endl;
}

void Cat::getName()
{
    std::cout << "Cat::getName()" << std::endl;
}

void Cat::getColor()
{
    std::cout << "Cat::getColor()" << std::endl;
}
