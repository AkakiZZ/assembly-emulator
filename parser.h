#include <iostream>
#include <string>
#include <regex>

#define memstore_one_byte(addr, n) (*((char*)((char*)memory + (addr))) = n) 
#define memstore_two_bytes(addr, n) (*((short*)((char*)memory + (addr))) = n)
#define memstore_four_bytes(addr, n) (*((int*)((char*)memory + (addr))) = n) 
#define memload_one_byte(addr) (int)(*((unsigned char*)((char*)memory + (addr))))
#define memload_two_bytes(addr) (int)(*((unsigned short*)((char*)memory + (addr))))
#define memload_four_bytes(addr) *((int*)((char*)memory + (addr)))

const static int SP_OP_OFFSET = 5;
const static std::string SP_OP_MATCH = "SP=SP";
const static std::string STORE_OP_MATCH = "M[]=";
const static std::string LOAD_OP_MATCH = "R=M[]";
const static std::string ALU_OP_MATCH_1 = "R=-"; 
const static std::string ALU_OP_MATCH_2 = "R=+"; 
const static std::string ALU_OP_MATCH_3 = "R=*"; 
const static std::string ALU_OP_MATCH_4 = "R=/"; 
const static std::string BRANCHES[6] = {"BLT", "BLE", "BEQ", "BNE", "BGT", "BGE"};
const static std::string BRANCH_MATCH = "B,,PC";

void error(int line_number);

void sp_op(const std::string instruction, int &SP, int &PC);
void store_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void load_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);
void alu_op(const std::string instruction, int &SP, int &PC, int &RV, void* &memory, int* &registers);