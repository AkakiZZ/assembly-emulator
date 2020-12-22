#include "parser.h"

using namespace std;

const static int SIZE_OF_INSTRUCTIONS = 4;

void error(int line_number) {
    cout << "Error at line #" << line_number << endl;
    exit(0);
}

bool is_number(const string s) {
    if (isdigit(s[0]) && s[0] == '0') return false;
    for(int i = 0; i < s.size(); i++)
        if (!isdigit(s[i])) return false;
    return true;
}

bool subsequence_helper(const string str1, const string str2, int m, int n) { 
    if (m == 0) return true; 
    if (n == 0) return false; 
    if (str1[m-1] == str2[n-1]) 
        return subsequence_helper(str1, str2, m-1, n-1); 
    return subsequence_helper(str1, str2, m, n-1); 
} 

bool is_subsequence(const string str1, const string str2) {
    return subsequence_helper(str1, str2, str1.length(), str2.length());
}

bool is_sp_op(const string instruction) {
    return instruction.substr(0, SP_OP_OFFSET) == SP_OP_MATCH;
}

string between_symbols(const string str, char ch1, char ch2) {
    int idx1 = str.find(ch1);
    int idx2 = str.find(ch2);
    return str.substr(idx1 + 1, idx2 - idx1 - 1);
}

bool is_store_op(const string instruction) {
    return is_subsequence(STORE_OP_MATCH, instruction) && instruction[0] == 'M';
}

bool is_load_op(const string instruction) {
    return is_subsequence(LOAD_OP_MATCH, instruction) && instruction[0] == 'R';
}

bool is_alu_op(const string instruction) {
    return (is_subsequence(ALU_OP_MATCH_1, instruction) ||
        is_subsequence(ALU_OP_MATCH_2, instruction) ||
        is_subsequence(ALU_OP_MATCH_3, instruction) ||
        is_subsequence(ALU_OP_MATCH_4, instruction)) &&
        instruction[0] == 'R';
} 

void sp_op(const string instruction, int &SP, int &PC) {
    if (!is_sp_op(instruction)) return;
    char op = instruction[SP_OP_OFFSET];
    string num = instruction.substr(SP_OP_OFFSET + 1, instruction.length() - SP_OP_OFFSET + 1);
    if (op == '-') SP -= stoi(num);
    else if (op == '+') SP += stoi(num);
    else error(GET_LINE(PC));
}

void store_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if(!is_store_op(instruction)) return;
    string arg = between_symbols(instruction, '[', ']');
    string to_store = between_symbols(instruction, '=', ';');
    if (arg == "SP") {
        if (is_number(to_store)) 
            memstore_four_bytes(SP, stoi(to_store));
        else if (to_store[0] == 'R')
            memstore_four_bytes(SP, registers[stoi(to_store.substr(1))]);
        else if (to_store == "SP")
            memstore_four_bytes(SP, SP);
        else if (to_store == "RV")
            memstore_four_bytes(SP, RV);
        else if (to_store == "PC")
            memstore_four_bytes(SP, PC);
        else
            error(GET_LINE(PC));
    } else if (arg[0] == 'R') {
        if (is_number(to_store)) 
            memstore_four_bytes(registers[stoi(arg.substr(1))], stoi(to_store));
        else if (to_store[0] == 'R')
            memstore_four_bytes(registers[stoi(arg.substr(1))], registers[stoi(to_store.substr(1))]);
        else if (to_store == "SP")
            memstore_four_bytes(registers[stoi(arg.substr(1))], SP);
        else if (to_store == "RV")
            memstore_four_bytes(registers[stoi(arg.substr(1))], RV);
        else if (to_store == "PC")
            memstore_four_bytes(registers[stoi(arg.substr(1))], PC);
        else
            error(GET_LINE(PC));
    } else if (is_number(arg)) {
        if (is_number(to_store)) 
            memstore_four_bytes(stoi(arg), stoi(to_store));
        else if (to_store[0] == 'R')
            memstore_four_bytes(stoi(arg), registers[stoi(to_store.substr(1))]);
        else if (to_store == "SP")
            memstore_four_bytes(stoi(arg), SP);
        else if (to_store == "RV")
            memstore_four_bytes(stoi(arg), RV);
        else if (to_store == "PC")
            memstore_four_bytes(stoi(arg), PC);
        else
            error(GET_LINE(PC));
    } else {
        error(GET_LINE(PC));
    }
}

void load_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_load_op(instruction)) return;
    int reg_num = stoi(between_symbols(instruction, 'R', '='));
    string arg = between_symbols(instruction, '[', ']');
    if (is_number(arg)) 
        registers[reg_num] = memload_four_bytes(stoi(arg));
    else if (arg[0] == 'R')
        registers[reg_num] = memload_four_bytes(registers[stoi(arg.substr(1))]);
    else if (arg == "SP")
        registers[reg_num] = memload_four_bytes(SP);
    else if (arg == "RV")
        registers[reg_num] = memload_four_bytes(RV);
    else if (arg == "PC")
        registers[reg_num] = memload_four_bytes(PC);
    else
        error(GET_LINE(PC));
}

void alu_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_alu_op(instruction)) return;
    int reg_num = stoi(between_symbols(instruction, 'R', '='));
    string arg1;
    string arg2;
    char op;
    if (instruction.find('+') != -1) {
        arg1 = between_symbols(instruction, '=', '+');
        arg2 = between_symbols(instruction, '+', ';');
        op = '+';
    } else if (instruction.find('-') != -1) {
        arg1 = between_symbols(instruction, '=', '-');
        arg2 = between_symbols(instruction, '-', ';');
        op = '-';
    } else if (instruction.find('*') != -1) {
        arg1 = between_symbols(instruction, '=', '*');
        arg2 = between_symbols(instruction, '*', ';');
        op = '*';
    } else if (instruction.find('/') != -1) {
        arg1 = between_symbols(instruction, '=', '/');
        arg2 = between_symbols(instruction, '/', ';');
        op = '/';
    } else {
        error(GET_LINE(PC));
    }
    int num1;
    int num2;
    if (arg1[0] == 'R') num1 = registers[stoi(arg1.substr(1))];
    else if (arg1 == "RV") num1 = RV;
    else if (is_number(arg1)) num1 = stoi(arg1);

    if (arg2[0] == 'R') num2 = registers[stoi(arg2.substr(1))];
    else if (arg2 == "RV") num2 = RV;
    else if (is_number(arg2)) num2 = stoi(arg2);

    if (op == '+') registers[reg_num] = num1 + num2;
    if (op == '-') registers[reg_num] = num1 - num2;
    if (op == '*') registers[reg_num] = num1 * num2;
    if (op == '/') registers[reg_num] = num1 / num2;
}
