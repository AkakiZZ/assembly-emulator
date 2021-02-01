#include <iostream>
#include <string>
#include <map>

#define memstore_one_byte(addr, n) (*((char*)((char*)memory + (addr))) = n) 
#define memstore_two_bytes(addr, n) (*((short*)((char*)memory + (addr))) = n)
#define memstore_four_bytes(addr, n) (*((int*)((char*)memory + (addr))) = n) 
#define memload_one_byte(addr) (int)(*((unsigned char*)((char*)memory + (addr))))
#define memload_two_bytes(addr) (int)(*((unsigned short*)((char*)memory + (addr))))
#define memload_four_bytes(addr) *((int*)((char*)memory + (addr)))

#define GET_LINE(PC) (PC / SIZE_OF_INSTRUCTIONS + 1)

const static int SP_OP_OFFSET = 5;
const static int NUM_BRANCHES = 6;
const static std::string SP_OP_MATCH = "SP=SP;";
const static std::string STORE_OP_MATCH = "M[]=;";
const static std::string LOAD_OP_MATCH = "R=M[];";
const static std::string ALU_OP_MATCH = "R=;";
const static std::string BRANCHES[6] = {"BLT", "BLE", "BEQ", "BNE", "BGT", "BGE"};
const static std::string BRANCH_MATCH = "B,,;";
const static std::string JMP_MATCH = "JMP";
const static std::string CALL_MATCH = "CALL<>";
const static std::string RET_MATCH = "RET;";

void error(int line_number);

bool is_sp_op(const std::string instruction);
bool is_jmp_op(const std::string instruction);
bool is_store_op(const std::string instruction);
bool is_load_op(const std::string instruction);
bool is_alu_op(const std::string instruction);
bool is_branch_op(const std::string instruction);
bool is_call_op(const std::string instruction);
bool is_ret_op(const std::string instruction);


void sp_op(const std::string instruction, int &SP, int &PC);
void store_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void load_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void alu_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void jmp_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void branch_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void call_op(const std::string instruction, std::map<std::string, int> functions, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void ret_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void def_op();

void error(int line_number);