#include "IntRegister.h"
// Register Constructor
IntRegister::IntRegister() {
    m_inUse = false;
    m_value = 0;
}
// Setter method for in-use value
void IntRegister::setInUse(bool inUse) {
    m_inUse = inUse;
}
// Setter method for register value
void IntRegister::setValue(int value) {
    m_value = value;
}
// Setter method for register name
void IntRegister::setName(string name) {
    m_name = name;
}
// Getter method for in-use value
bool IntRegister::getInUse() {
    return m_inUse;
}
// Getter method for register value
int IntRegister::getValue() {
    return m_value;
}

// Getter method for register value
string IntRegister::getName() {
    return m_name;
}

// Performs add operation
void IntRegister::add(int r1, int r2) {
    m_value = r1 + r2;
}

// Performs add operation with immediate
void IntRegister::addi(int r1, int immediate) {
    m_value = r1 + immediate;
}

// Performs subtraction operation
void IntRegister::sub(int r1, int r2) {
    m_value = r1 - r2;
}

// Performs multiplication operation
void IntRegister::mul(int r1, int r2) {
    m_value = r1 * r2;
}

// Performs division operation
void IntRegister::div(int r1, int r2) {
    m_value = r1 / r2;
}

// Performs load operation
void IntRegister::load(int value) {
    m_value = value;
}

// Displays name and value for end output of register
void IntRegister::displayValue() {
    cout << m_name << " = " << m_value << endl;
}