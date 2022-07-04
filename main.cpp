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
    for(long unsigned int i = 0; i < sizeof(floatingPointRegisters)/sizeof(floatingPointRegisters[0]); i++) {
        tempName = "F" + to_string(i);
        floatingPointRegisters[i].setName(tempName);
    }

    for(long unsigned int j = 0; j < sizeof(integerRegisters)/sizeof(integerRegisters[0]); j++) {
        tempName = "$" + to_string(j);
        integerRegisters[j].setName(tempName);
    }
    int lines = findLines(fileName);
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
                instructions[line].setStoredRegister(temp);
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
    int addersInUse = NumAdders;
    int multipliersInUse = NumMultipliers;
    int dividersInUse = NumDividers;
    int integerUnitsInUse = NumIntegerUnits;
    int cycleCount = 1;
    int instructionIndex = 0; //index for current instruction in array
    bool finished = false;
    while (finished == false) {
        string currInstruction = instructions[instructionIndex].getInstruction();
        if (currInstruction == "ADD" || currInstruction == "ADDI" || currInstruction == "ADD.D" 
        || currInstruction == "SUB" || currInstruction == "SUB.D") {
            if (addersInUse > 0) { //check for any available adders
                addersInUse--;
                instructions[instructionIndex].setIssue(cycleCount);
            }
        } else if (currInstruction == "MUL.D") {
            if (multipliersInUse > 0) { //check for any available adders
                multipliersInUse--;
                instructions[instructionIndex].setIssue(cycleCount);
            }
        }

        instructionIndex++;
        cycleCount++;
    }
    
}

// REMAKE WITH TEMPLATES AND CHECK ORDER OF OPERATIONS!!!!!!!!!!
void add(IntRegister dest, IntRegister r1, IntRegister r2) {
    dest.setValue(r1.getValue() + r2.getValue());
}

void subtract(IntRegister dest, IntRegister r1, IntRegister r2) {
    dest.setValue(r1.getValue() - r2.getValue());
}

void multiply(IntRegister dest, IntRegister r1, IntRegister r2) {
    dest.setValue(r1.getValue() * r2.getValue());
}

void divide(IntRegister dest, IntRegister r1, IntRegister r2) {
    dest.setValue(r1.getValue() / r2.getValue());
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