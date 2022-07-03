#ifndef INT_REGISTER_H
#define INT_REGISTER_H
#include <iostream>
using namespace std;

class IntRegister {
private:
    bool m_inUse;
    int m_value;
    string m_name;
public:
    IntRegister();
    void setInUse(bool inUse);
    void setValue(int value);
    void setName(string name);
    bool getInUse();
    int getValue();
    void displayValue();
};

#endif