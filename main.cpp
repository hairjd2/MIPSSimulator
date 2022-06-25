#include <iostream>
#include <fstream>
#include <string>
#include "Line.h"
using namespace std;

// Constant values
const int MEMORY_SIZE = 19;
const int FP_REGISTER_COUNT = 32;
const int INT_REGISTER_COUNT = 32;

const int FP_ADD_SUB_CYCLES = 2;
const int MUL_CYCLES = 10;
const int DIV_CYCLES = 40;
const int INT_UNIT_CYCLES = 1; 

//Memory and register values
int memoryTable[MEMORY_SIZE] = {45, 12, 0, 0, 10, 135, 254, 127, 18, 4, 55, 8, 2, 98, 13, 5, 233, 158, 167};
float floatingPointRegisters[FP_REGISTER_COUNT];
int integerRegisters[INT_REGISTER_COUNT];
int clockCycle = 0;

//Helper Functions
void readFile(string fileName, int lines);
int findLines(string fileName);
void outputResult();

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
    
// Once file is valid, readfile is called after getting number of lines from findLines
    readFile(fileName, findLines(fileName));
// Outputs final register values and table
    outputResult();
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

void readFile(string fileName, int lines) {
// Opens file and initializes temp string for each word inputted and the array of Line objects
    ifstream inputFile;
    inputFile.open(fileName.c_str());
    string temp;
    Line instructions[lines];

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
                if(temp == "L.D" || temp == "S.D") {
                    count++;
                    memory = true;
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
// Will keep track of the program and calculate how long for each cycle it will take
    for(int i = 0; i < lines; i++) {
        // This for loop for now just outputs all of the fields that every object has as a test
        // I'm gonna implement another function to keep track of the delays and stuff and what stages each one is at.
        instructions[i].displayScoreBoardLine();
    }
    cout << endl;
}

void outputResult() {
// Outputs all floating point register values
    cout << "Floating Point Register Values:" << endl;
    for(int i = 0; i < FP_REGISTER_COUNT; i++) {
        cout << "F" << i << ": " << floatingPointRegisters[i] << endl;
    }

// Outputs all floating point register values
    cout << endl << "Integer Register Values:" << endl;
    for(int j = 0; j < FP_REGISTER_COUNT; j++) {
        cout << "$" << j << ": " << integerRegisters[j] << endl;
    }
}