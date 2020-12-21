#include "parser.h"

using namespace std;

const static int SIZE_OF_INSTRUCTIONS = 4;

void error(int line_number) {
    cout << "Error at line #" << line_number << endl;
    exit(0);
}

bool is_number(string s) {
    if (isdigit(s[0]) && s[0] == '0') return false;
    for(int i = 0; i < s.size(); i++)
        if (!isdigit(s[i])) return false;
    return true;
}

bool subsequence_helper(string str1, string str2, int m, int n) { 
    if (m == 0) return true; 
    if (n == 0) return false; 
    if (str1[m-1] == str2[n-1]) 
        return subsequence_helper(str1, str2, m-1, n-1); 
    return subsequence_helper(str1, str2, m, n-1); 
} 

bool is_subsequence(string str1, string str2) {
    return subsequence_helper(str1, str2, str1.length(), str2.length());
}

bool is_sp_op(const string instruction) {
    return instruction.substr(0, SP_OP_OFFSET) == SP_OP_MATCH;
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
    else error(PC / SIZE_OF_INSTRUCTIONS + 1);
}

void store_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if(!is_store_op(instruction)) return;
    regex r1("\\[(.*)\\]");
    regex r2("\\=(.*)");
    smatch m;
    regex_search(instruction, m, r1);
    string arg = (m.str(0).substr(1)).substr(0, m.str(0).length() - 2);
    regex_search(instruction, m, r2);
    string to_store = m.str(1);
    if (arg == "SP") {
        if (is_number(to_store)) 
            memstore_four_bytes(SP, stoi(to_store));
        else if (to_store[0] == 'R')
            memstore_four_bytes(SP, registers[stoi(m.str(1).substr(1))]);
        else if (to_store == "SP")
            memstore_four_bytes(SP, SP);
        else if (to_store == "RV")
            memstore_four_bytes(SP, RV);
        else if (to_store == "PC")
            memstore_four_bytes(SP, PC);
        else
            error(PC / SIZE_OF_INSTRUCTIONS + 1);
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
            error(PC / SIZE_OF_INSTRUCTIONS + 1);
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
            error(PC / SIZE_OF_INSTRUCTIONS + 1);
    } else {
        error(PC / SIZE_OF_INSTRUCTIONS + 1);
    }
}

void load_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_load_op(instruction)) return;
    regex r1("^(.*?)\\=");
    regex r2("\\[(.*)\\]");
    smatch m;
    regex_search(instruction, m, r1);
    int reg_num = stoi((m.str(0).substr(0, m.str(0).length() - 1)).substr(1));
    regex_search(instruction, m, r2);
    string arg = (m.str(0).substr(1)).substr(0, m.str(0).length() - 2);
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
        error(PC / SIZE_OF_INSTRUCTIONS + 1);
}

void alu_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_alu_op(instruction)) return;
}
