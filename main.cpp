#include "parser.h"
#include "reader.h"
#include <stdlib.h>

using namespace std;

const static int SIZE_OF_INSTRUCTIONS = 4;
const static int VIRTUAL_MEMORY_SIZE = 1000000;
const static int NUM_OF_REGISTERS = 1000;

void execute_instruction(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void execute_instructions(const vector<string> &instructions);

int main() {
    ifstream file;
    file >> std::ws;
    get_input(file);
    vector<string> instructions;
    read_file(file, instructions);
    execute_instructions(instructions);
    file.close();
    return 0;
}

void execute_instruction(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    sp_op(instruction, SP, PC);
    store_op(instruction, SP, PC, RV, memory, registers);
    load_op(instruction, SP, PC, RV, memory, registers);
    alu_op(instruction, SP, PC, RV, memory, registers);
}

void execute_instructions(const vector<string> &instructions) {
    int PC = 0;
    int SP = VIRTUAL_MEMORY_SIZE - 4;
    int RV;
    void* memory = malloc(VIRTUAL_MEMORY_SIZE);
    int* registers = (int*)malloc(NUM_OF_REGISTERS * sizeof(int));

    for (PC = 0; PC < instructions.size() * SIZE_OF_INSTRUCTIONS; PC += SIZE_OF_INSTRUCTIONS) {
        //cout << instructions[PC / SIZE_OF_INSTRUCTIONS] << endl;
        execute_instruction(instructions[PC / SIZE_OF_INSTRUCTIONS], SP, PC, RV, memory, registers);
    }

    free(memory);
    free(registers);
}


