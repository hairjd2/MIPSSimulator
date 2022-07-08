/* John Hair and Nicholas Battista */
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
    string getName();
    void displayValue();
    void add(int r1, int r2);
    void addi(int r1, int immediate);
    void sub(int r1, int r2);
    void mul(int r1, int r2);
    void div(int r1, int r2);
    void load(int value);
};

#endif