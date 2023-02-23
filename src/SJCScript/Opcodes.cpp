//Main Header
#include "Opcodes.h"

static struct
{
	CString str;
	Opcode opcode;
} g_Opcodes[] =
{
	{"add", OPCODE_ADD},
	{"addf", OPCODE_ADDF},
	{"anew", OPCODE_ANEW},
	{"call", OPCODE_CALL},
	{"divf", OPCODE_DIVF},
	{"i2f", OPCODE_I2F},
	{"inc", OPCODE_INC},
	{"je", OPCODE_JE},
	{"jl", OPCODE_JL},
	{"jle", OPCODE_JLE},
	{"jmp", OPCODE_JMP},
	{"jne", OPCODE_JNE},
	{"jz", OPCODE_JZ},
	{"ldc", OPCODE_LDC},
	{"mov", OPCODE_MOV},
	{"mova", OPCODE_MOVA},
	{"mul", OPCODE_MUL},
	{"pop", OPCODE_POP},
	{"push", OPCODE_PUSH},
	{"ret", OPCODE_RET},
	{"sel", OPCODE_SEL},
	{"sub", OPCODE_SUB},
	{"sysc", OPCODE_SYSC},

};

Opcode GetOpcode(CString &string)
{
	repeat(sizeof(g_Opcodes)/sizeof(g_Opcodes[0]), i)
	{
		if(string == g_Opcodes[i].str)
			return g_Opcodes[i].opcode;
	}

	return OPCODE_ILLEGAL;
}

CString GetOpcodeIdentifier(Opcode opc)
{
	repeat(sizeof(g_Opcodes)/sizeof(g_Opcodes[0]), i)
	{
		if(opc == g_Opcodes[i].opcode)
			return g_Opcodes[i].str;
	}

	return "INVALID OPCODE";
}

#define CMP(sysCallId) if(string == #sysCallId) return sysCallId;

ESysCallId GetSysCallId(CString &string)
{
	CMP(SYSCALL_PRINTI);
	CMP(SYSCALL_PRINTF);
	CMP(SYSCALL_PRINTS);
	CMP(SYSCALL_RAND);

	return SYSCALL_ILLEGAL;
}

#undef CMP
#define CMP(sysCallId) case sysCallId: return #sysCallId;

CString GetSysCallIdentifier(ESysCallId id)
{
	switch(id)
	{
		CMP(SYSCALL_PRINTI);
		CMP(SYSCALL_PRINTF);
		CMP(SYSCALL_PRINTS);
		CMP(SYSCALL_RAND);
	}

	return "INVALID SYSCALL ID";
}

bool IsValidSMOpcode(uint32 value)
{
	if(value >= OPCODE_ADD && value <= OPCODE_SYSC)
		return true;
	return false;
}