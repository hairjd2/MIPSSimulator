#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Line.h"
#include "IntRegister.h"
#include "FloatRegister.h"
using namespace std;

// Constant values
const int MEMORY_SIZE = 19;
const int FP_REGISTER_COUNT = 32;
const int INT_REGISTER_COUNT = 32;

int NumAdders = 0;
int NumMultipliers = 0;
int NumDividers = 0;
int NumIntegerUnits = 0;

int lines = 0;

//Memory and register values
int memoryTable[MEMORY_SIZE] = {45, 12, 0, 0, 10, 135, 254, 127, 18, 4, 55, 8, 2, 98, 13, 5, 233, 158, 167};
FloatRegister floatingPointRegisters[FP_REGISTER_COUNT];
IntRegister integerRegisters[INT_REGISTER_COUNT]; 
int clockCycle = 0;

//Helper Functions
int findLines(string fileName);
void readFile(string fileName, int lines, Line *instructions);
void run(Line *instructions, int lines);
void outputResult(Line *instructions, int lines);

// Stage Functions
int issue(int cycleCount, int programCounter, Line *instructions);
void readOperands(int cycleCount, Line *instructions);
void executionComplete(int cycleCount, Line *instructions);
void writeResult(int cycleCount, Line *instructions);

// Arithmetic Functions
template <typename T>
void store(int address, T r1);

int main() {
// Main function: takes in initial inputs and runs the helper functions
// Gets file name
    string fileName;
    cout << "What file would you like to input: ";
    cin >> fileName;

// Checks for error in name until user provides a valid file name
    ifstream inputFile;
    inputFile.open(fileName);
    while (inputFile.fail()) {
        cerr << "Error opening file!" << endl << "What file would you like to input: ";
        cin >> fileName;
        inputFile.open(fileName);
    }
    inputFile.close();

    cout << "Enter the number of adders: ";
    cin >> NumAdders;
    cout << "Enter the number of multipliers: ";
    cin >> NumMultipliers;
    cout << "Enter the number of dividers: ";
    cin >> NumDividers;
    cout << "Enter the number of integer units: ";
    cin >> NumIntegerUnits;

    string tempName = "";
    for(int i = 0; i < FP_REGISTER_COUNT; i++) {
        tempName = "F" + to_string(i);
        floatingPointRegisters[i].setName(tempName);
    }

    for(int j = 0; j < INT_REGISTER_COUNT; j++) {
        tempName = "$" + to_string(j);
        integerRegisters[j].setName(tempName);
    }
    lines = findLines(fileName);
    Line *instructions = new Line[lines];

// Once file is valid, readfile is called after getting number of lines from findLines
    readFile(fileName, lines, instructions);

// Runs the instructions given and finds the cycle times for each stage
    run(instructions, lines); //may need to pass NumAdders, NumDividers, etc

// Outputs final register values and table
    outputResult(instructions, lines);
}

int findLines(string fileName) {
// Opens file and sets number of lines to 0
    ifstream inputFile;
    inputFile.open(fileName);
    int lines = 0;
    if(inputFile.is_open()) {
// If file can be opened, the num of lines is set to 1
        lines++;
        char currChar;

// For each new line character, num of lines is incremented
// returns when file is at the end
        while(inputFile) {
            inputFile.get(currChar);
            if(currChar == '\n') {
                lines++;
            }
        }
    }
    inputFile.close();
    return lines;
}

void readFile(string fileName, int lines, Line *instructions) {
// Opens file and initializes temp string for each word inputted and the array of Line objects
    ifstream inputFile;
    inputFile.open(fileName.c_str());
    string temp;

    if (inputFile.is_open()) {
// The point of count is to keep track at which "word" the loop is on on the line
        int count = 0;
        int line = 0;
        bool memory = false;

        while(inputFile >> temp) {
// When count is 0, that means it reads in the first "word", which is the instruction
            if(count == 0) {
                instructions[line].setInstruction(temp);
// If the instruction is load or store, that means it only will have two arguments, one including an address, meaning the count must be added twice instead of once
// Else, just add count once
                if(temp.at(0) == 'L' || (temp.at(0) == 'S' && temp.at(1) != 'U')) {
                    count++;
                    memory = true;
                    instructions[line].setAccessesMem(memory);
                }
            }
// If count is 1 or count is 2 and it is a load or store instruction, then the read in is on the second "word", a.k.a. the "stored register"
            else if(count == 1 || (count == 2 && memory)) {
                if(instructions[line].getInstruction() == "LI") {
                    instructions[line].setImmediate(stoi(temp));
                }else {
                    instructions[line].setStoredRegister(temp);
                }
            }
// If count is 2 but not a load or store instruction, then it is the first register in the arithmetic input
            else if(count == 2) {
                instructions[line].setRegister1(temp);
            }
            else if(count == 3) {
// If count is 3 and a load or store instruction, then the temp string holds the address
                if(memory) {
                    instructions[line].setAddress(temp);
                }
                else if(instructions[line].getInstruction() == "ADDI") {
                    instructions[line].setImmediate(stoi(temp));
                }

                else {
// If count is 3 and not memory related, temp holds the second register of the arithmetic argument
                    instructions[line].setRegister2(temp);
                }
            }
            count++;
// Once the count has reached 3, it has reached the end of the line and must keep track of the next line
            if(count > 3) {
                count = 0;
                line++;
                memory = false;
            }
            
        }
       
    }
    inputFile.close();
    cout << endl;
}

void run(Line *instructions, int lines) {
    /* Steps for scoreboarding process
    Choose next instruction
    Check if instruction of same type is being run
    If it is, don't issue. If it's not, check if the dest reg is already being written to
    If it is, don't issue. If not, issue the instruction

    Check each instruction currently issued to see if it can be read:
    is m_reg1 or m_reg2 the same as the dest reg of an instruction currently executing?
    if yes, do not read. If no, read

    Check each instruction in execution if the curr cycle == cycle execution will finish
    if they're equal, mark execution as done. If not, continue

    Check each instruction for writeback
    if execution is marked as done, writeback
    */
    int cycleCount = 1;
    int programCounter = 0; //index for current instruction in array
    bool finished = false;
    while (finished == false) {
        programCounter = issue(cycleCount, programCounter, instructions);
        readOperands(cycleCount, instructions);
        executionComplete(cycleCount, instructions);
        writeResult(cycleCount, instructions);
        finished = true;
        for (int i = 0; i < lines; i++) {
            if (instructions[i].getWriteResult() == 0) {
                finished = true;
            }
        }
        cycleCount++;
    }    
}

int issue(int cycleCount, int programCounter, Line *instructions) {
    string currInstruction = instructions[programCounter].getInstruction();
    int index = int(instructions[programCounter].getStoredRegister().at(1));
    if (currInstruction == "ADD" || currInstruction == "ADD.D" || currInstruction == "SUB" || currInstruction == "SUB.D" || currInstruction == "ADDI") {
        if (NumAdders > 0) { //check for any available adders
                
            if (instructions[programCounter].getStoredRegister().at(0) == 'F') {
                if (!floatingPointRegisters[index].getInUse()) {
                    NumAdders--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(2);
                    return programCounter++;
                }
            } else {
                if (!integerRegisters[index].getInUse()) {
                    NumAdders--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(2);
                    return programCounter++;
                }
            }
        } 
    } else if (currInstruction == "MUL.D") {
        if (NumMultipliers > 0) { //check for any available multipliers
            if (instructions[programCounter].getStoredRegister().at(0) == 'F') {
                if (!floatingPointRegisters[index].getInUse()) {
                    NumMultipliers--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(10);
                    return programCounter++;
                }
            } else {
                if (!integerRegisters[index].getInUse()) {
                    NumMultipliers--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(10);
                    return programCounter++;
                }
            }
        }
    } else if (currInstruction == "DIV.D") {
        if (NumDividers > 0) { //check for any available dividers
            if (instructions[programCounter].getStoredRegister().at(0) == 'F') {
                if (!floatingPointRegisters[index].getInUse()) {
                    NumDividers--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(40);
                    return programCounter++;
                }
            } else {
                if (!integerRegisters[index].getInUse()) {
                    NumDividers--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(40);
                    return programCounter++;
                }
            }
        }
    } else if (currInstruction == "L.D" || currInstruction == "LW" || currInstruction == "S.D" || currInstruction == "SW" || currInstruction == "LI") {
        if (NumIntegerUnits > 0) { //check for any available integer units
            NumIntegerUnits--;
            instructions[programCounter].setIssue(cycleCount);
            instructions[programCounter].setExecutionTime(1);
            return programCounter++;
        }
    }
    return programCounter;
}

void readOperands(int cycleCount, Line *instructions) {
    bool goodToRead = true;
    for (int i = 0; i < lines; i++) {
        if (instructions[i].getIssue() > 0 && instructions[i].getReadOperands() == 0) { //has been issued, not read
            for (int j = 0; j < lines; j++) {
                if (instructions[j].getReadOperands() > 0 && instructions[j].getExecution() == 0) { //currently executing
                    if (instructions[i].getRegister1() == instructions[j].getStoredRegister() && 
                    instructions[i].getRegister2() == instructions[j].getStoredRegister()) { //no RAW issue
                        goodToRead = false;
                    }
                }
            }           
            if (goodToRead) {
                instructions[i].setReadOperands(cycleCount);
                instructions[i].setExecution(instructions[i].getExecutionTime() + cycleCount);
            }
        }
    }
}

void executionComplete(int cycleCount, Line *instructions) {
    for (int i = 0; i < lines; i++) {
        if (instructions[i].getExecution() == cycleCount) {
            instructions[i].setWriteResult(cycleCount + 1);
        }
    }
    
}

void writeResult(int cycleCount, Line *instructions) {
    for (int i = 0; i < lines; i++) {
        int destIndex = int(instructions[i].getStoredRegister().at(1));
        
        // write functions to find values like address, registers etc.
        string str;
        bool track = false;
        for (int j = 0; j < int(instructions[i].getAddress().length()); j++) {
            if (instructions[i].getAddress().at(j) == ')') {
                track = false;
            }
            if (track == true) {
                str = str + instructions[i].getAddress().at(j);
            }
            if (instructions[i].getAddress().at(j) == '(') {
                track = true;
            }
        }
        int address = stoi(str);

        string str2;
        bool track2 = true;      
        for (int j = 0; j < int(instructions[i].getAddress().length()); j++) {
            if (instructions[i].getAddress().at(j) == '(') {
                track2 = false;
            }
            if (track2 == true) {
                str2 = str2 + instructions[i].getAddress().at(j);
            }
        }
        address += stoi(str2);
        int memValue = memoryTable[address];
        
        if (instructions[i].getWriteResult() == cycleCount) {
            if(instructions[i].getInstruction() == "L.D") {
                floatingPointRegisters[destIndex].load(memValue);
            }else if(instructions[i].getInstruction() == "S.D") {
                store<FloatRegister>(address, floatingPointRegisters[destIndex]);
            }else if(instructions[i].getInstruction() == "LI") {
                int r1Index = int(instructions[i].getRegister1().at(1));
                floatingPointRegisters[destIndex].load(instructions[i].getImmediate());
            }else if(instructions[i].getInstruction() == "LW") {
                integerRegisters[destIndex].load(memValue);
            }else if(instructions[i].getInstruction() == "SW") {
                store<IntRegister>(address, integerRegisters[r1Index]);
            }else if(instructions[i].getInstruction() == "ADD") {
                integerRegisters[destIndex].add(integerRegisters[r1Index].getValue(), integerRegisters[r2Index].getValue());
            }else if(instructions[i].getInstruction() == "ADDI") {
                integerRegisters[destIndex].add(integerRegisters[r1Index].getValue(), instructions[i].getImmediate());
            }else if(instructions[i].getInstruction() == "ADD.D") {
                floatingPointRegisters[destIndex].add(floatingPointRegisters[r1Index].getValue(), floatingPointRegisters[r2Index].getValue());
            }else if(instructions[i].getInstruction() == "SUB.D") {
                floatingPointRegisters[destIndex].sub(floatingPointRegisters[r1Index].getValue(), floatingPointRegisters[r2Index].getValue());
            }else if(instructions[i].getInstruction() == "SUB") {
                integerRegisters[destIndex].sub(integerRegisters[r1Index].getValue(), integerRegisters[r2Index].getValue());
            }else if(instructions[i].getInstruction() == "MUL.D") {
                floatingPointRegisters[destIndex].mul(floatingPointRegisters[r1Index].getValue(), floatingPointRegisters[r2Index].getValue());
            }else if(instructions[i].getInstruction() == "DIV.D") {
                floatingPointRegisters[destIndex].div(floatingPointRegisters[r1Index].getValue(), floatingPointRegisters[r2Index].getValue());
            }
        }
    }
}

template <typename T>
void store(int address, T r1) {
    memoryTable[address] = r1.getValue();
}

void outputResult(Line *instructions, int lines) {
// Outputs all floating point register values
    cout << "Floating Point Register Values:" << endl;
    for(int i = 0; i < FP_REGISTER_COUNT; i++) {
        floatingPointRegisters[i].displayValue();
    }

// Outputs all floating point register values
    cout << endl << "Integer Register Values:" << endl;
    for(int j = 0; j < FP_REGISTER_COUNT; j++) {
        integerRegisters[j].displayValue();
    }
    
    // Will keep track of the program and calculate how long for each cycle it will take
    cout << endl << "Scoreboard Results:" << endl;
    for(int i = 0; i < lines; i++) {
        // This for loop for now just outputs all of the fields that every object has as a test
        instructions[i].displayScoreBoardLine();
    }

    delete [] instructions;
}