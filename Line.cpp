#include "Line.h"
// Line Constructor
Line::Line() {
    m_instruction = "";
}
// Setter method for instruction value
void Line::setInstruction(string instruction) {
    m_instruction = instruction;
}
// Setter method for stored register value
void Line::setStoredRegister(string storedRegister) {
    m_storedRegister = storedRegister;
}
// Setter method for first register value
void Line::setRegister1(string register1) {
    m_register1 = register1;
}
// Setter method for second register value
void Line::setRegister2(string register2) {
    m_register2 = register2;
}
// Setter method for address value
void Line::setAddress(string address) {
    m_address = address;
}
// Getter method for instruction value
string Line::getInstruction() {
    return m_instruction;
}
// Getter method for stored register value
string Line::getStoredRegister() {
    return m_storedRegister;
}
// Getter method for address value
string Line::getAddress() {
    return m_address;
}
// Getter method for first register value
string Line::getRegister1() {
    return m_register1;
}
// Getter method for second register value
string Line::getRegister2() {
    return m_register2;
}