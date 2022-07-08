/* John Hair and Nicholas Battista */
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

const int ADDER_CYCLES = 2;
const int MUL_CYCLES = 10;
const int DIV_CYCLES = 40;
const int INT_CYCLES = 1;

const int zeroASCII = 48;

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
int getRegister1Index(Line *instructions, int instruction);
int getRegister2Index(Line *instructions, int instruction);
int getAddress(Line *instructions, int instruction);
int getAddressStore(Line *instructions, int instruction);

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
        floatingPointRegisters[i].setInUse(false);
    }

    for(int j = 0; j < INT_REGISTER_COUNT; j++) {
        tempName = "$" + to_string(j);
        integerRegisters[j].setName(tempName);
        integerRegisters[j].setInUse(false);
    }
    lines = findLines(fileName);
    Line *instructions = new Line[lines];

// Once file is valid, readfile is called after getting number of lines from findLines
    readFile(fileName, lines, instructions);

// Runs the instructions given and finds the cycle times for each stage
    run(instructions, lines); //may need to pass NumAdders, NumDividers, etc

// Outputs final register values and table
    outputResult(instructions, lines);
    delete [] instructions;
    return 0;
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
                if(instructions[line].getInstruction().at(0) == 'S' && memory) {
                    string corrected = temp;
                    corrected.pop_back();
                    instructions[line].setRegister2(corrected);
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
                if(memory && instructions[line].getInstruction() != "LI") {
                    if(instructions[line].getInstruction().at(0) == 'S') {
                        instructions[line].setStoredRegister(temp);
                    } else {
                        instructions[line].setAddress(temp);
                    }
                }
                else if(instructions[line].getInstruction() == "ADDI" || instructions[line].getInstruction() == "LI") {
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
    /* Handles the main scoreboard cycling
    One while loop = one scoreboard cycle */
    int cycleCount = 1;
    int programCounter = 0; //index for current instruction in array
    bool finished = false;
    while (finished == false) {
        if(programCounter < lines) { //prevents from attempting to issue a nonexistent instruction
            programCounter = issue(cycleCount, programCounter, instructions);
        }
        readOperands(cycleCount, instructions);
        executionComplete(cycleCount, instructions);
        writeResult(cycleCount, instructions);
        finished = true;
        for (int i = 0; i < lines; i++) {
            if (instructions[i].getWriteResult() == 0) { //if any instructions haven't been written, not finished
                finished = false;
            }
        }
        cycleCount++;
    }
}

int issue(int cycleCount, int programCounter, Line *instructions) {
    /* Steps for issuing
    Choose next instruction
    Check if instruction of same type is being run
    If it is, don't issue. If it's not, check if the dest reg is already being written to
    If it is, don't issue. If not, issue the instruction */
    
    string currInstruction = instructions[programCounter].getInstruction();
    int index = int(instructions[programCounter].getStoredRegister().at(1)) - zeroASCII;
    if (currInstruction == "ADD" || currInstruction == "ADD.D" || currInstruction == "SUB" || currInstruction == "SUB.D" || currInstruction == "ADDI") {
        if (NumAdders > 0) { //check for any available adders    
            if (instructions[programCounter].getStoredRegister().at(0) == 'F') { //checks if storing in FP reg
                if (!floatingPointRegisters[index].getInUse()) { //if dest reg isn't being written to by another instruction
                    NumAdders--; //takes up an adder
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(2); //sets length of execution (2 cycles for adder)
                    floatingPointRegisters[index].setInUse(true);
                    ++programCounter; //moves PC to next instruction
                }
            } else {
                if (!integerRegisters[index].getInUse()) { //if storing in Int reg OR 
                    NumAdders--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(2);
                    integerRegisters[index].setInUse(true);
                    ++programCounter;
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
                    floatingPointRegisters[index].setInUse(true);
                    ++programCounter;
                }
            } else {
                if (!integerRegisters[index].getInUse()) {
                    NumMultipliers--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(10);
                    integerRegisters[index].setInUse(true);
                    ++programCounter;
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
                    floatingPointRegisters[index].setInUse(true);
                    ++programCounter;
                }
            } else {
                if (!integerRegisters[index].getInUse()) {
                    NumDividers--;
                    instructions[programCounter].setIssue(cycleCount);
                    instructions[programCounter].setExecutionTime(40);
                    integerRegisters[index].setInUse(true);
                    ++programCounter;
                }
            }
        }
    } else if (currInstruction == "L.D" || currInstruction == "LW" || currInstruction == "LI" || currInstruction == "S.D" || currInstruction == "SW") {
        if (NumIntegerUnits > 0) { //check for any available integer units
            NumIntegerUnits--;
            instructions[programCounter].setIssue(cycleCount);
            instructions[programCounter].setExecutionTime(1);
            floatingPointRegisters[index].setInUse(true);
            ++programCounter;
        }
    }

    return programCounter; //updates PC in run method
}

void readOperands(int cycleCount, Line *instructions) {
    /*Check each instruction currently issued to see if it can be read:
    is m_reg1 or m_reg2 the same as the dest reg of an instruction currently executing?
    if yes, do not read. If no, read*/
    bool goodToRead = true;
    for (int i = 0; i < lines; i++) {
        if (instructions[i].getIssue() != cycleCount && instructions[i].getIssue() > 0 && instructions[i].getReadOperands() == 0) { //has been issued, not read
            for (int j = 0; j < lines; j++) {
                if (instructions[j].getWriteResult() == cycleCount || (instructions[j].getReadOperands() > 0 && instructions[j].getWriteResult() == 0)) { //only uses currently executing instructions
                    if (instructions[i].getRegister1() == instructions[j].getStoredRegister() || 
                    instructions[i].getRegister2() == instructions[j].getStoredRegister()) { //RAW issue, don't read
                        goodToRead = false;
                    }
                }
            }           
            if (goodToRead) {
                instructions[i].setReadOperands(cycleCount);
                instructions[i].setExecution(instructions[i].getExecutionTime() + cycleCount); //Sets execution time ahead of time to be checked in executionComplete
            }
        }
    }
}

void executionComplete(int cycleCount, Line *instructions) {
    /*
    Check each instruction in execution if the curr cycle == cycle execution will finish
    if they're equal, mark execution as done. If not, continue
    */
    
    for (int i = 0; i < lines; i++) {
        if (instructions[i].getExecution() == cycleCount) {
            instructions[i].setWriteResult(cycleCount + 1); //sets write cycle result ahead of time to be checked in writeResult
        }
    }
}

void writeResult(int cycleCount, Line *instructions) {
    /* 
    Check each instruction for writeback
    if execution is marked as done, writeback
    */
    
    for (int i = 0; i < lines; i++) {
        int destIndex = int(instructions[i].getStoredRegister().at(1)) - zeroASCII;
        
        if (instructions[i].getWriteResult() == cycleCount) { //if instruction ready to write
            if(instructions[i].getInstruction() == "L.D") {
                int memValue = memoryTable[getAddress(instructions, i)]; //gets value from memory
                floatingPointRegisters[destIndex].load(memValue);
                floatingPointRegisters[destIndex].setInUse(false); //dest reg no longer being written to
                NumIntegerUnits++; //frees up an integer unit
            }else if(instructions[i].getInstruction() == "S.D") {
                store<FloatRegister>(getAddressStore(instructions, i), floatingPointRegisters[getRegister2Index(instructions, i)]); //stores value from reg2 into a mem address
                floatingPointRegisters[destIndex].setInUse(false);
                NumIntegerUnits++;
            }else if(instructions[i].getInstruction() == "LI") {
                floatingPointRegisters[destIndex].load(instructions[i].getImmediate()); //loads an immediate, memValue not needed
                NumIntegerUnits++;
            }else if(instructions[i].getInstruction() == "LW") { //same as L.D instruction but for int reg
                int memValue = memoryTable[getAddress(instructions, i)];
                integerRegisters[destIndex].load(memValue);
                integerRegisters[destIndex].setInUse(false);
                NumIntegerUnits++;
            }else if(instructions[i].getInstruction() == "SW") { //same as S.D instruction but for int reg
                store<IntRegister>(destIndex, integerRegisters[getRegister2Index(instructions, i)]);
                integerRegisters[destIndex].setInUse(false);
                NumIntegerUnits++;
            }else if(instructions[i].getInstruction() == "ADD") { // Adds r1 and r2 of line and stores into dest register
                integerRegisters[destIndex].add(integerRegisters[getRegister1Index(instructions, i)].getValue(), integerRegisters[getRegister2Index(instructions, i)].getValue());
                integerRegisters[destIndex].setInUse(false);
                NumAdders++;
            }else if(instructions[i].getInstruction() == "ADDI") { // same as ADD but adds immeidate value instead of register value
                integerRegisters[destIndex].add(integerRegisters[getRegister1Index(instructions, i)].getValue(), instructions[i].getImmediate());
                integerRegisters[destIndex].setInUse(false);
                NumAdders++;
            }else if(instructions[i].getInstruction() == "ADD.D") { // same as ADD but for floating point registers
                floatingPointRegisters[destIndex].add(floatingPointRegisters[getRegister1Index(instructions, i)].getValue(), floatingPointRegisters[getRegister2Index(instructions, i)].getValue());
                floatingPointRegisters[destIndex].setInUse(false);
                NumAdders++;
            }else if(instructions[i].getInstruction() == "SUB.D") { // Subtracts r1 and r2 of line and stores into dest register
                floatingPointRegisters[destIndex].sub(floatingPointRegisters[getRegister1Index(instructions, i)].getValue(), floatingPointRegisters[getRegister2Index(instructions, i)].getValue());
                floatingPointRegisters[destIndex].setInUse(false);
                NumAdders++;
            }else if(instructions[i].getInstruction() == "SUB") { //same as SUB.D but for integer registers
                integerRegisters[destIndex].sub(integerRegisters[getRegister1Index(instructions, i)].getValue(), integerRegisters[getRegister2Index(instructions, i)].getValue());
                integerRegisters[destIndex].setInUse(false);
                NumAdders++;
            }else if(instructions[i].getInstruction() == "MUL.D") { // Multiplies r1 and r2 of line and stores into dest register
                floatingPointRegisters[destIndex].mul(floatingPointRegisters[getRegister1Index(instructions, i)].getValue(), floatingPointRegisters[getRegister2Index(instructions, i)].getValue());
                floatingPointRegisters[destIndex].setInUse(false);
                NumMultipliers++;
            }else if(instructions[i].getInstruction() == "DIV.D") { // Divides r1 and r2 of line and stores into dest register
                floatingPointRegisters[destIndex].div(floatingPointRegisters[getRegister1Index(instructions, i)].getValue(), floatingPointRegisters[getRegister2Index(instructions, i)].getValue());
                floatingPointRegisters[destIndex].setInUse(false);
                NumDividers++;
            }
        }
    }
}

// Separate function for updating value in the memory table
template <typename T>
void store(int address, T r1) {
    memoryTable[address] = r1.getValue();
}

// Returns the index of the register object
int getRegister1Index(Line *instructions, int instruction) {
    return int(instructions[instruction].getRegister1().at(1)) - zeroASCII;
}

// Returns the index of the register object
int getRegister2Index(Line *instructions, int instruction) {
    return int(instructions[instruction].getRegister2().at(1)) - zeroASCII;
}

// Parses the address string and returns a singular int to be used as an index
int getAddress(Line *instructions, int instruction) {
    // write functions to find values like address, registers etc.
        string str;
        bool track = false;
        for (int j = 0; j < int(instructions[instruction].getAddress().length()); j++) {
            if (instructions[instruction].getAddress().at(j) == ')') {
                track = false;
            }
            if (track == true) {
                str = str + instructions[instruction].getAddress().at(j);
            }
            if (instructions[instruction].getAddress().at(j) == '(') {
                track = true;
            }
        }

        int address = stoi(str);

        string str2;
        bool track2 = true;      
        for (int j = 0; j < int(instructions[instruction].getAddress().length()); j++) {
            if (instructions[instruction].getAddress().at(j) == '(') {
                track2 = false;
            }
            if (track2 == true) {
                str2 = str2 + instructions[instruction].getAddress().at(j);
            }
        }
        address += stoi(str2);
        return address;
}

// Same as function above but for store instructions
int getAddressStore(Line *instructions, int instruction) {
    // write functions to find values like address, registers etc.
        string str;
        bool track = false;
        for (int j = 0; j < int(instructions[instruction].getStoredRegister().length()); j++) {
            if (instructions[instruction].getStoredRegister().at(j) == ')') {
                track = false;
            }
            if (track == true) {
                str = str + instructions[instruction].getStoredRegister().at(j);
            }
            if (instructions[instruction].getStoredRegister().at(j) == '(') {
                track = true;
            }
        }

        int address = stoi(str);

        string str2;
        bool track2 = true;      
        for (int j = 0; j < int(instructions[instruction].getStoredRegister().length()); j++) {
            if (instructions[instruction].getStoredRegister().at(j) == '(') {
                track2 = false;
            }
            if (track2 == true) {
                str2 = str2 + instructions[instruction].getStoredRegister().at(j);
            }
        }
        address += stoi(str2);
        return address;
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
}