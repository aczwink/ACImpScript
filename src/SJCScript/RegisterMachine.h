#pragma once
//SJCLib
#include <SJCLib.h>
//Local
#include "Opcodes.h"
//Namespaces
using namespace SJCLib;
//Definitions
#define REGVM_NREGISTERS REG_T6+1

enum
{
	RM_OPCODE_ILLEGAL,
	RM_OPCODE_ADD,
	RM_OPCODE_ANEW,
	RM_OPCODE_CALL,
	RM_OPCODE_DIVF,
	RM_OPCODE_I2F,
	RM_OPCODE_JE,
	RM_OPCODE_JL,
	RM_OPCODE_JLE,
	RM_OPCODE_JMP,
	RM_OPCODE_JZ,
	RM_OPCODE_LDC,
	RM_OPCODE_MOV,
	RM_OPCODE_MOVA,
	RM_OPCODE_MUL,
	RM_OPCODE_POP,
	RM_OPCODE_PUSH,
	RM_OPCODE_RET,
	RM_OPCODE_SEL,
	RM_OPCODE_SUB,
	RM_OPCODE_SYSC,
};

enum ERegister
{
	REG_ILLEGAL = -1,
	REG_R0,
	REG_A0,
	REG_A1,
	REG_S0,
	REG_S1,
	REG_T0,
	REG_T1,
	REG_T2,
	REG_T3,
	REG_T4,
	REG_T5,
	REG_T6
};

Opcode RM_GetOpcode(CString &string);
CString RM_GetOpcodeIdentifier(Opcode op);
ERegister RM_GetRegister(CString &string);
CString RM_GetRegisterIdentifier(ERegister reg);
bool RM_IsValidOpcode(uint32 value);