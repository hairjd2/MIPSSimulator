#ifndef FLOAT_REGISTER_H
#define FLOAT_REGISTER_H
#include <iostream>
using namespace std;

class FloatRegister {
private:
    bool m_inUse;
    float m_value;
    string m_name;
public:
    FloatRegister();
    void setInUse(bool inUse);
    void setValue(float value);
    void setName(string name);
    bool getInUse();
    float getValue();
    void displayValue();
};

#endif