#include <iostream>
#include <fstream>
#include <string>
#include "Line.h"
using namespace std;

int MEMORY_TABLE[19] = {45, 12, 0, 0, 10, 135, 254, 127, 18, 4, 55, 8, 2, 98, 13, 5, 233, 158, 167};

void readFile(string fileName, int lines);

int main() {
    string fileName;
    cout << "What file would you like to input: ";
    cin >> fileName;
    int lines = 9;
    readFile(fileName, lines);
}

void readFile(string fileName, int lines) {
    ifstream inputFile;
    inputFile.open(fileName.c_str());
    string temp;
    Line instructions[lines];

    if (inputFile.is_open()) {
        int count = 0;
        int line = 0;
        bool memory = false;

        while(inputFile >> temp) {
            if(count == 0) {
                instructions[line].setInstruction(temp);
                if(temp == "L.D" || temp == "S.D") {
                    count++;
                    memory = true;
                }
            }
            else if(count == 1 || (count == 2 && memory)) {
                instructions[line].setStoredRegister(temp);
            }
            else if(count == 2) {
                instructions[line].setRegister1(temp);
            }
            else if(count == 3) {
                if(memory) {
                    instructions[line].setAddress(temp);
                }
                else {
                    instructions[line].setRegister2(temp);
                }
            }

            count++;

            if(count > 3) {
                count = 0;
                line++;
                memory = false;
            }
            
        }
       
    }
    inputFile.close();

    for(int i = 0; i < lines; i++) {
        cout << instructions[i].getInstruction() << instructions[i].getStoredRegister() << instructions[i].getAddress() << instructions[i].getRegister1() << instructions[i].getRegister2() << endl;
    }
}