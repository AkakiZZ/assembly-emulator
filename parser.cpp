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

bool is_jmp_op(const string instruction) {
    return instruction.substr(0, 3) == JMP_MATCH;
}

string between_symbols(const string str, char ch1, char ch2) {
    int idx1 = str.find(ch1);
    int idx2 = -1;
    if (ch1 == ch2) 
        idx2 = (str.substr(idx1 + 1)).find(ch2) + idx1 + 1;
    if (idx2 == -1)
        idx2 = str.find(ch2);
    return str.substr(idx1 + 1, idx2 - idx1 - 1);
}

int jump_helper(const string arg, int* &registers, int &PC, int &RV) {
    int new_pc;
    if (is_number(arg))
        new_pc = stoi(arg);
    else if (arg[0] == 'R' && is_number(arg.substr(1))) 
        new_pc = registers[stoi(arg.substr(1))];
    else if (arg.substr(0, 3) == "PC-" && is_number(arg.substr(3))) 
        new_pc = PC - stoi(arg.substr(3));
    else if (arg.substr(0, 3) == "PC+" && is_number(arg.substr(3))) 
        new_pc = PC + stoi(arg.substr(3));
    else if (arg.substr(0, 2) == "RV" && arg.length() == 2) 
        new_pc = RV;
    else 
        error(GET_LINE(PC));
    if ((new_pc % 4 != 0 || new_pc == 0) && new_pc >= 0) error(GET_LINE(PC));
    return new_pc;
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

bool is_branch_op(const string instruction) {
    return instruction[0] == 'B' && is_subsequence(BRANCH_MATCH, instruction);
}

bool is_call_op(const string instruction) {
    return instruction.substr(0, 5) == CALL_MATCH.substr(0, 5)
    && is_subsequence(CALL_MATCH, instruction);
}

bool is_ret_op(const string instruction) {
    return instruction == RET_MATCH;
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
    string arg = between_symbols(instruction, '[', ']');
    if (instruction[0] == 'R' && between_symbols(instruction, 'R', '=') == "V") {
        if (is_number(arg)) 
            RV = memload_four_bytes(stoi(arg));
        else if (arg[0] == 'R')
            RV = memload_four_bytes(registers[stoi(arg.substr(1))]);
        else if (arg == "SP")
            RV = memload_four_bytes(SP);
        else if (arg == "PC")
            RV = memload_four_bytes(PC);
        else
            error(GET_LINE(PC));
    } else if (instruction[0] == 'R' && is_number(between_symbols(instruction, 'R', '='))) {
        int reg_num = stoi(between_symbols(instruction, 'R', '='));
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
    } else {
        error(GET_LINE(PC));
    }
    
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

void jmp_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_jmp_op(instruction)) return;
    string arg = between_symbols(instruction, 'P', ';');
    int new_pc = jump_helper(arg, registers, PC, RV);
    PC = new_pc - 4;
}

void branch_op(const string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_branch_op(instruction)) return;
    int branch_idx = -1;
    string branch = instruction.substr(0, 3);
    for (int i = 0; i < NUM_BRANCHES; i++) {
        if (BRANCHES[i] == branch) {
            branch_idx = i;
            break;
        }
    } 
    if (branch_idx == -1) error(GET_LINE(PC));
    string arg1 = between_symbols(instruction, branch[branch.size() - 1], ',');
    string arg2 = between_symbols(instruction, ',', ',');
    int num1, num2;
    string jump_arg = between_symbols(instruction.substr(instruction.find(',') + 1), ',', ';');
    if (arg1 == "RV" && arg1.length() == 2) 
        num1 = RV;
    else if (arg1[0] == 'R' && is_number(arg1.substr(1))) 
        num1 = registers[stoi(arg1.substr(1))];
    else if (is_number(arg1)) 
        num1 = stoi(arg1);
    else 
        error(GET_LINE(PC));

    if (arg2 == "RV" && arg2.length() == 2) 
        num2 = RV;
    else if (arg2[0] == 'R' && is_number(arg2.substr(1))) 
        num2 = registers[stoi(arg2.substr(1))];
    else if (is_number(arg2)) 
        num2 = stoi(arg2);
    else 
        error(GET_LINE(PC));
    
    if (branch_idx == 0 && num1 < num2) {
        PC = jump_helper(jump_arg, registers, PC, RV);
    } else if (branch_idx == 1 && num1 <= num2) {
        PC = jump_helper(jump_arg, registers, PC, RV);
    } else if (branch_idx == 2 && num1 == num2) {
        PC = jump_helper(jump_arg, registers, PC, RV);
    } else if (branch_idx == 3 && num1 != num2) {
        PC = jump_helper(jump_arg, registers, PC, RV);
    } else if (branch_idx == 4 && num1 > num2) {
        PC = jump_helper(jump_arg, registers, PC, RV);
    } else if (branch_idx == 5 && num1 >= num2) {
        PC = jump_helper(jump_arg, registers, PC, RV);
    } 
}

void call_op(const string instruction, map<string, int> functions, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_call_op(instruction)) return;
    string arg = between_symbols(instruction, '<', '>');
    SP -= sizeof(int);
    memstore_four_bytes(SP, PC); //saved pc 
    if (arg[0] == 'R' && is_number(arg.substr(1))) {
        PC = registers[stoi(arg.substr(1))];
    } else if (arg == "RV") {
        PC = RV;
    } else {
        if (functions.count(arg)) {
            PC = functions[arg];
        } else {
            error(GET_LINE(PC));
        }
    }
    
}

void ret_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers) {
    if (!is_ret_op(instruction)) return;
    PC = memload_four_bytes(SP);
    SP += sizeof(int);
}