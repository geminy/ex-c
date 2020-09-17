#include <iostream>
#include "animal.h"
#include "cat.h"
#include "dog.h"

template<typename T>
void CheckPointer(T obj, std::string str)
{
    if (!obj) {
        std::cout << "Pointer is null: "  << str <<  std::endl;
    }
}

template<typename T>
void DeleteObject(T obj)
{
    if (obj) {
        delete obj;
        obj = 0;
    }
}

int main()
{
    // constructor father > child
    std::cout << "constructor--------------------" << std::endl;
    Animal *pAnimal = new Animal();
    CheckPointer(pAnimal, "pAnimal");
    Cat *pCat = new Cat();
    CheckPointer(pCat, "pCat");
    Dog *pDog = new Dog();
    CheckPointer(pDog, "pDog");

    // upward-cast child > father auto
    std::cout << "upward-cast--------------------" << std::endl;
    Animal *pAniCat = pCat;
    CheckPointer(pAniCat, "pAniCat");
    Animal *pAniDog = pDog;
    CheckPointer(pAniDog, "pAniDog");

    // downward-cast father > child manual
    // static_cast ok
    // dynamic_cast fail
    std::cout << "downward-cast--------------------" << std::endl;
    Cat *pCatAni = static_cast<Cat*>(pAnimal);
    CheckPointer(pCatAni, "pCatAni");
    Dog *pDogAni = static_cast<Dog*>(pAnimal);
    CheckPointer(pDogAni, "pDogAni");

    // inherit
    std::cout << "inherit--------------------" << std::endl;
    pAnimal->getKind();
    pCat->getKind();
    pDog->getKind();
    pAniCat->getKind();
    pAniDog->getKind();
    pCatAni->getKind();
    pDogAni->getKind();
    pDogAni->getKind();

    // override
    std::cout << "override--------------------" << std::endl;
    pAnimal->getName();
    pCat->getName();
    pDog->getName();
    pAniCat->getName();
    pAniDog->getName();
    pCatAni->getName();
    pDogAni->getName();

    // override virtual
    std::cout << "override virtual--------------------" << std::endl;
    pAnimal->getColor();
    pCat->getColor();
    pDog->getColor();
    pAniCat->getColor();
    pAniDog->getColor();
    pCatAni->getColor();
    pDogAni->getColor();

    // destructor chld > father
    std::cout << "destructor--------------------" << std::endl;
    DeleteObject(pAnimal);
    DeleteObject(pCat);
    DeleteObject(pDog);

    return 0;
}

