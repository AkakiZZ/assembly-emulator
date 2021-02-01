#include "reader.h"

using namespace std;

void get_input(ifstream &file) {
    string filename;
    cout << "Input filename: ";
    cin >> filename;
    file.open(filename);
    while(file.fail()) {
        cout << "File doesn't exist\nInput filename: ";
        cin >> filename;
        file.open(filename);
    }
}

void read_file(ifstream &file, vector<string> &instructions) {
    string line;
    while (getline(file >> std::ws, line)) {
        string without_spaces = "";
        for (int i = 0; i < line.length(); i++) {
            if (line[i] != ' ') 
                without_spaces += line[i];
        }
        if (line.length() > 0)
            instructions.push_back(without_spaces);
    }
}