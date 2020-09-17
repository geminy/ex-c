#ifndef REFERENCECOUNT
#define REFERENCECOUNT

// Reference count, copy-on-write
#include <iostream>
#include <string>

using namespace std;

class Dog
{
public:
    // Dogs can only be created on the heap
    static Dog* Make(const string& name)
    {
        return new Dog(name);
    }

    Dog(const Dog& d)
        : m_name(d.m_name + " copy"), m_refCount(1)
    {
        cout << "Dog copy-constructor: " << *this << endl;
    }

    ~Dog()
    {
        cout << "Deleting Dog: " << *this << endl;
    }

    void attach()
    {
        ++m_refCount;
        cout << "Attached Dog: " << *this << endl;
    }

    void detach()
    {
        if (m_refCount != 0) {
            cout << "Detaching Dog: " << *this << endl;
            // Destroy object if no one is using it
            if (--m_refCount == 0) {
                delete this;
            }
        }
    }

    // Conditionally copy this Dog
    // Call before modifying the Dog
    // assign resulting pointer to your Dog*
    Dog* unalias()
    {
        cout << "Unaliasing Dog: " << *this << endl;
        // Don't duplicate if not aliased
        if (m_refCount == 1) {
            return this;
        }
        --m_refCount;
        // Use copy-constructor to duplicate
        return new Dog(*this);
    }

    void rename(const string& newName)
    {
        m_name = newName;
        cout << "Dog renamed to: " << *this << endl;
    }

    friend ostream& operator<<(ostream& os, const Dog& d)
    {
        return os << "[" << d.m_name << "], rc = " << d.m_refCount;
    }

private:
    Dog(const string& name)
        : m_name(name), m_refCount(1)
    {
        cout << "Creating Dog: " << *this << endl;
    }

    // Prevent assignment
    Dog& operator=(const Dog& d);

private:
    string m_name;
    int m_refCount;
};

class DogHouse
{
public:
    DogHouse(Dog* dog, const string& house)
        : m_pDog(dog), m_houseName(house)
    {
        cout << "Created DogHouse: " << *this << endl;
    }

    DogHouse(const DogHouse& dh)
        : m_pDog(dh.m_pDog), m_houseName("copy-constructed " + dh.m_houseName)
    {
        // not deep-copy
        // using reference count
        m_pDog->attach();
        cout << "DogHouse copy-constructor: " << *this << endl;
    }

    DogHouse& operator=(const DogHouse& dh)
    {
        // Check for self-assignment
        if (&dh != this) {
            m_houseName = dh.m_houseName + " assigned";
            // Clean up what you're using first
            m_pDog->detach();
            // not deep-copy
            // using reference count
            m_pDog = dh.m_pDog;
            m_pDog->attach();
        }
        cout << "DogHouse operator=  : "  << *this << endl;
        return *this;
    }

    ~DogHouse()
    {
        cout << "DogHouse destructor: " << *this << endl;
        // Clean up what you're using
        m_pDog->detach();
    }

    void renameHouse(const string& newName)
    {
        m_houseName = newName;
    }

    void unalias()
    {
        m_pDog = m_pDog->unalias();
    }

    // Copy-on-write, Anytime you modify the contents of the pointer
    // you must first unalias it
    void renameDog(const string& newName)
    {
        unalias();
        m_pDog->rename(newName);
    }

    // ... or when you allow someone else access
    Dog* getDog()
    {
        unalias();
        return m_pDog;
    }

    friend ostream& operator<<(ostream& os, const DogHouse& dh)
    {
        return os << "[" << dh.m_houseName << "] contains " << *dh.m_pDog;
    }

private:
    Dog* m_pDog;
    string m_houseName;
};

#endif // REFERENCECOUNT
