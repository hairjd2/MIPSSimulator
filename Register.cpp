#include "Register.h"
// Register Constructor
Register::Register() {
    m_inUse = false;
    m_value = 0;
}
// Setter method for in-use value
void Register::setInUse(bool inUse) {
    m_inUse = inUse;
}
// Setter method for register value
void Register::setValue(int value) {
    m_value = value;
}
// Getter method for in-use value
bool Register::getInUse() {
    return m_inUse;
}
// Getter method for register value
T Register::getValue() {
    return m_value;
}