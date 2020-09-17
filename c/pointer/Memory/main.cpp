#include "bitcopy.h"
#include "valuecopy.h"
#include "referencesimple.h"
#include "referencecount.h"

int main()
{
//    cout << "Using simple copy" << endl;
//    BitCopy a;
//    BitCopy b(a);

//    cout << "Using deep copy" << endl;
//    ValueCopy c;
//    ValueCopy d(c);

    cout << "Using reference count" << endl;
    B e;
    B f(e);

//    DogHouse fidos(Dog::Make("Fido"), "FidoHouse");
//    DogHouse spots(Dog::Make("Spot"), "SpotHouse");
//    cout << "Entering copy-construction" << endl;
//    DogHouse bobs(fidos);
//    cout << "After copy-construction bobs" << endl;
//    cout << "fidos: " << fidos << endl;
//    cout << "spots: " << spots << endl;
//    cout << "bobs: " << bobs << endl;
//    cout << "Entering spots = fidos" << endl;
//    spots = fidos;
//    cout << "After spots = fidos" << endl;
//    cout << "Entering self-assignment" << endl;
//    cout << "bobs: " << bobs << endl;
//    // Comment out the following lines
//    cout << "Entering rename(\"Bob\")" << endl;
//    bobs.getDog()->rename("Bob");
//    cout << "After rename(\"Bob\")" << endl;

    return 0;
}

