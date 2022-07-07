#include "FloatRegister.h"
// Register Constructor
FloatRegister::FloatRegister() {
    m_inUse = false;
    m_value = 0.0;
}
// Setter method for in-use value
void FloatRegister::setInUse(bool inUse) {
    m_inUse = inUse;
}
// Setter method for register value
void FloatRegister::setValue(float value) {
    m_value = value;
}
void FloatRegister::setName(string name) {
    m_name = name;
}
// Getter method for in-use value
bool FloatRegister::getInUse() {
    return m_inUse;
}
// Getter method for register value
float FloatRegister::getValue() {
    return m_value;
}

void FloatRegister::add(float r1, float r2) {
    m_value = r1 + r2;
}

void FloatRegister::addi(float r1, int immediate) {
    m_value = r1 + float(immediate);
}

void FloatRegister::sub(float r1, float r2) {
    m_value = r1 - r2;
}

void FloatRegister::mul(float r1, float r2) {
    m_value = r1 * r2;
}

void FloatRegister::div(float r1, float r2) {
    m_value = r1 / r2;
}

void FloatRegister::load(int value) {
    m_value = float(value);
}

void FloatRegister::displayValue() {
    cout << m_name << " = " << m_value << endl;
}

