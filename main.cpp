#include "parser.h"
#include "reader.h"
#include <stdlib.h>
#include <map>

using namespace std;

const static int SIZE_OF_INSTRUCTIONS = 4;
const static int VIRTUAL_MEMORY_SIZE = 1000000;
const static int NUM_OF_REGISTERS = 1000;
const static string DEFINE_KEYWORD = "DEF<";
const static string END_KEYWORD = "END_DEF;";

void execute_instruction(const string instruction, map<string, int> functions, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void execute_instructions(const vector<string> &instructions, map<string, int> functions);
void get_functions(const vector<string> &instructions, map<string, int> &functions);

int main() {
    ifstream file;
    file >> std::ws;
    get_input(file);
    vector<string> instructions;
    map<string, int> functions;
    read_file(file, instructions);
    get_functions(instructions, functions);
    execute_instructions(instructions, functions);
    file.close();
    return 0;
}

void execute_instruction(const string instruction, map<string, int> functions, 
                        int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    sp_op(instruction, SP, PC);
    store_op(instruction, SP, PC, RV, memory, registers);
    load_op(instruction, SP, PC, RV, memory, registers);
    alu_op(instruction, SP, PC, RV, memory, registers);
    jmp_op(instruction, SP, PC, RV, memory, registers);
    branch_op(instruction, SP, PC, RV, memory, registers);
    call_op(instruction, functions, SP, PC, RV, memory, registers);
    ret_op(instruction, SP, PC, RV, memory, registers);
}

void get_functions(const vector<string> &instructions, map<string, int> &functions) {
    bool complete = true;
    for (int i = 0; i < instructions.size(); i++) {
        if (instructions[i].length() >= 4 && instructions[i].substr(0, 4) == DEFINE_KEYWORD) {
            complete = !complete;
            functions[instructions[i].substr(4, instructions[i].length() - 6)] = i * SIZE_OF_INSTRUCTIONS;
        }
        if (instructions[i] == END_KEYWORD) {
            complete = !complete;
        }
    }
    if (!complete) abort();
}

void execute_instructions(const vector<string> &instructions, map<string, int> functions) {
    int PC = 0;
    int SP = VIRTUAL_MEMORY_SIZE - 4;
    int RV;
    void* memory = malloc(VIRTUAL_MEMORY_SIZE);
    int* registers = (int*)malloc(NUM_OF_REGISTERS * sizeof(int));
    
    for (PC = 0; PC < instructions.size() * SIZE_OF_INSTRUCTIONS; PC += SIZE_OF_INSTRUCTIONS) {
        
        while (instructions[PC / SIZE_OF_INSTRUCTIONS].substr(0, 4) == DEFINE_KEYWORD) {
            while (instructions[PC / SIZE_OF_INSTRUCTIONS] != END_KEYWORD) {
                PC += SIZE_OF_INSTRUCTIONS;
            }
            PC += SIZE_OF_INSTRUCTIONS;
        }
        execute_instruction(instructions[PC / SIZE_OF_INSTRUCTIONS], functions, SP, PC, RV, memory, registers);
    }
    cout << "Program Ran Succesfully" << endl;
    free(memory);
    free(registers);

}


