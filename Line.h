#ifndef LINE_H
#define LINE_H
#include <iostream>
using namespace std;

class Line {
private:
    string m_instruction;
    string m_register1;
    string m_register2;
    string m_storedRegister;
    string m_address;
    int m_issue;
    int m_readOperands;
    int m_execution;
    int m_writeResult;

public:
    Line();
    void setInstruction(string Instruction);
    void setStoredRegister(string storedRegister);
    void setRegister1(string register1);
    void setRegister2(string register2);
    void setAddress(string address);
    string getInstruction();
    string getStoredRegister();
    string getAddress();
    string getRegister1();
    string getRegister2();
    void setIssue(int issue);
    void setReadOperands(int readOperands);
    void setExecution(int execution);
    void setWriteResult(int writeResult);
    void displayScoreBoardLine();
};

#endif