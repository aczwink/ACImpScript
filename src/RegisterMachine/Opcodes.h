#define NUMBER_OF_REGISTERS 255
/*
r0-r31 result registers
a0-a31 argument registers
s0-s31 save registers
t0-t158 temporary registers
*/
#define REG_A0 32

enum EOpcode
{
	OPCODE_ADD,
	OPCODE_CALL,
	OPCODE_HLT,
	OPCODE_JE,
	OPCODE_JLE,
	OPCODE_JZ,
	OPCODE_L32,
	OPCODE_L64,
	OPCODE_MOV,
	OPCODE_MUL,
	OPCODE_POP,
	OPCODE_PUSH,
	OPCODE_RET,
	OPCODE_SUB,
	OPCODE_SYSC,
};