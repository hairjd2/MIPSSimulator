#include "Line.h"

Line::Line() {
    m_instruction = "";
}

void Line::setInstruction(string instruction) {
    m_instruction = instruction;
}
 
void Line::setStoredRegister(string storedRegister) {
    m_storedRegister = storedRegister;
}

void Line::setRegister1(string register1) {
    m_register1 = register1;
}

void Line::setRegister2(string register2) {
    m_register2 = register2;
}

void Line::setAddress(string address) {
    m_address = address;
}

string Line::getInstruction() {
    return m_instruction;
}

string Line::getStoredRegister() {
    return m_storedRegister;
}

string Line::getAddress() {
    return m_address;
}

string Line::getRegister1() {
    return m_register1;
}

string Line::getRegister2() {
    return m_register2;
}