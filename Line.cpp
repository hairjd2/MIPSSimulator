/* John Hair and Nicholas Battista */
#include "Line.h"
// Line Constructor
Line::Line() {
    m_instruction = "";
    m_immediate = 0;
    m_issue = 0;
    m_readOperands = 0;
    m_execution = 0;
    m_writeResult = 0;
    m_accessesMem = false;
}
// Setter method for instruction value
void Line::setInstruction(string instruction) {
    m_instruction = instruction;
}
// Setter method for stored register value
void Line::setStoredRegister(string storedRegister) {
    storedRegister.pop_back();
    m_storedRegister = storedRegister;
}
// Setter method for first register value
void Line::setRegister1(string register1) {
    register1.pop_back();
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
// Setter method for immediate value
void Line::setImmediate(int immediate) {
    m_immediate = immediate;
}
// Setter method for load/store boolean
void Line::setAccessesMem(bool accessesMem) {
    m_accessesMem = accessesMem;
}

void Line::setExecutionTime(int executionTime) {
    m_executionTime = executionTime;
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

// Getter method for issue cycle time
int Line::getIssue() {
    return m_issue;
}

// Getter method for read operands cycle time
int Line::getReadOperands() {
    return m_readOperands;
}

// Getter method for execution completion cycle time
int Line::getExecution() {
    return m_execution;
}

// Getter method for write back cycle time
int Line::getWriteResult() {
    return m_writeResult;
}

// Getter method for immediate value
int Line::getImmediate() {
    return m_immediate;
}

// Getter method for load/store boolean
bool Line::getAccessesMem() {
    return m_accessesMem;
}

// Getter method for instruction to execute
int Line::getExecutionTime() {
    return m_executionTime;
}

// Setter method for issue cycle time
void Line::setIssue(int issue) {
    m_issue = issue;
}

// Getter method for read operands cycle time
void Line::setReadOperands(int readOperands) {
    m_readOperands = readOperands;
}

// Getter method for execution completion cycle time
void Line::setExecution(int execution) {
    m_execution = execution;
}

// Getter method for write back cycle time
void Line::setWriteResult(int writeResult) {
    m_writeResult = writeResult;
}

// Formatted output for each line
void Line::displayScoreBoardLine() {
    if(m_instruction.at(0) == 'L') {
        cout << m_instruction << " " << m_storedRegister << ", " << m_address << "\t";
    }else if(m_instruction == "S.D" || m_instruction == "SW") {
        cout << m_instruction << " " << m_register2 << ", " << m_storedRegister << ")";
    }
    else if(m_instruction == "ADDI") {
        cout << m_instruction << " " << m_storedRegister << ", " << m_register1 << ", " << m_immediate;
    }
    else {
        cout << m_instruction << " " << m_storedRegister << ", " << m_register1 << ", " << m_register2;
    }
    
    if(m_instruction == "ADDI") {
        cout << "\t\tDest: " << m_storedRegister;
    }
    else if(m_instruction == "S.D" || m_instruction == "SW") {
        cout << "\t\tDest: " << m_storedRegister << ")";
    }
    else {
        cout << "\tDest: " << m_storedRegister;
    }

    if(m_accessesMem) {
        if(m_instruction == "LI") {
            cout << "\tS1: " << 0;
            cout << "\tS2: " << m_immediate;
        } else if(m_instruction == "S.D" || m_instruction == "SW") {
            cout << "\tS1: " << 0;
            cout << "\tS2: " << m_register2;
        } else {
            cout << "\tS1: " << 0;
            cout << "\tS2: " << m_address;
        }
    } else if(m_instruction == "ADDI") {
        cout << "\tS1: " << m_register1;
        cout << "\tS2: " << m_immediate << "\t";
    } else {
        cout << "\tS1: " << m_register1;
        cout << "\tS2: " << m_register2 << "\t";
    }

    if (m_instruction == "S.D" || m_instruction == "SW") {
        cout << "\t\tIssue: " << m_issue;
    } else {
        cout << "\tIssue: " << m_issue;
    }

    cout << "\tRead Operands: " << m_readOperands;
    cout << "\tExecution: " << m_execution;
    cout << "\tWrite Results: " << m_writeResult;
    cout << endl;
}