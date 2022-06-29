#ifndef REGISTER_H
#define REGISTER_H
#include <iostream>
using namespace std;

template <class T>
class Register {
private:
    bool m_inUse;
    T m_value;
public:
    Register();
    void setInUse(bool inUse);
    void setValue(T value);
    bool getInUse();
    T getValue();
};

#endif