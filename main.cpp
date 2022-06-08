#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void readFile(string fileName);

int main() {
    string fileName;
    cout << "What file would you like to input: ";
    cin >> fileName;
    readFile(fileName);
}

void readFile(string fileName) {
    ifstream inputFile;
    inputFile.open(fileName);
    if (inputFile.is_open()) {
        char currChar;
        while (inputFile && currChar != '\n') {
            currChar = inputFile.get();
            cout << currChar;
            cout << endl;
        }
    }
    inputFile.close();
}