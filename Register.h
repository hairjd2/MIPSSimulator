#ifndef REGISTER_H
#define REGISTER_H
#include <iostream>
using namespace std;

template <typename T>
class Register {
private:
    bool m_inUse;
    T m_value;
public:
    Register();
    void setInUse(bool inUse);
    void setValue(int value);
    bool getInUse();
    int getValue();
};

#endif