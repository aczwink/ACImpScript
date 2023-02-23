//Main Header
#include "RegisterMachine.h"

static struct
{
	CString str;
	Opcode opcode;
} g_Opcodes[] =
{
	{"add", RM_OPCODE_ADD},
	{"anew", RM_OPCODE_ANEW},
	{"call", RM_OPCODE_CALL},
	{"divf", RM_OPCODE_DIVF},
	{"i2f", RM_OPCODE_I2F},
	{"je", RM_OPCODE_JE},
	{"jl", RM_OPCODE_JL},
	{"jle", RM_OPCODE_JLE},
	{"jmp", RM_OPCODE_JMP},
	{"jz", RM_OPCODE_JZ},
	{"ldc", RM_OPCODE_LDC},
	{"mov", RM_OPCODE_MOV},
	{"mova", RM_OPCODE_MOVA},
	{"mul", RM_OPCODE_MUL},
	{"pop", RM_OPCODE_POP},
	{"push", RM_OPCODE_PUSH},
	{"ret", RM_OPCODE_RET},
	{"sel", RM_OPCODE_SEL},
	{"sub", RM_OPCODE_SUB},
	{"sysc", RM_OPCODE_SYSC},
};

static struct
{
	CString str;
	ERegister reg;
} g_Registers[] =
{
	{"r0", REG_R0},
	{"a0", REG_A0},
	{"a1", REG_A1},
	{"s0", REG_S0},
	{"s1", REG_S1},
	{"t0", REG_T0},
	{"t1", REG_T1},
	{"t2", REG_T2},
	{"t3", REG_T3},
	{"t4", REG_T4},
	{"t5", REG_T5},
	{"t6", REG_T6}
};

Opcode RM_GetOpcode(CString &string)
{
	repeat(sizeof(g_Opcodes)/sizeof(g_Opcodes[0]), i)
	{
		if(string == g_Opcodes[i].str)
			return g_Opcodes[i].opcode;
	}
	
	return RM_OPCODE_ILLEGAL;
}

CString RM_GetOpcodeIdentifier(Opcode opc)
{
	repeat(sizeof(g_Opcodes)/sizeof(g_Opcodes[0]), i)
	{
		if(opc == g_Opcodes[i].opcode)
			return g_Opcodes[i].str;
	}

	return "INVALID OPCODE";
}

ERegister RM_GetRegister(CString &string)
{
	repeat(sizeof(g_Registers)/sizeof(g_Registers[0]), i)
	{
		if(string == g_Registers[i].str)
			return g_Registers[i].reg;
	}
	
	return REG_ILLEGAL;
}

CString RM_GetRegisterIdentifier(ERegister reg)
{
	repeat(sizeof(g_Registers)/sizeof(g_Registers[0]), i)
	{
		if(reg == g_Registers[i].reg)
			return g_Registers[i].str;
	}
	
	return REG_ILLEGAL;
}

bool RM_IsValidOpcode(uint32 value)
{
	if(value >= RM_OPCODE_ADD && value <= RM_OPCODE_SYSC)
		return true;
	return false;
}