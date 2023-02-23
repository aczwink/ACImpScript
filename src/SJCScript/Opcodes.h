#pragma once
//SJCLib
#include <SJCLib.h>
//Namespaces
using namespace SJCLib;

typedef uint32 Opcode;

enum
{
	OPCODE_ILLEGAL,
	OPCODE_ADD,
	OPCODE_ADDF,
	OPCODE_ANEW,
	OPCODE_CALL,
	OPCODE_DIVF,
	OPCODE_I2F,
	OPCODE_INC,
	OPCODE_JE,
	OPCODE_JL,
	OPCODE_JLE,
	OPCODE_JMP,
	OPCODE_JNE,
	OPCODE_JZ,
	OPCODE_LDC,
	OPCODE_MOV,
	OPCODE_MOVA,
	OPCODE_MUL,
	OPCODE_POP,
	OPCODE_PUSH,
	OPCODE_RET,
	OPCODE_SEL,
	OPCODE_SUB,
	OPCODE_SYSC
};

enum ESysCallId
{
	SYSCALL_ILLEGAL,
	SYSCALL_PRINTI,
	SYSCALL_PRINTF,
	SYSCALL_PRINTS,
	SYSCALL_RAND,
};

Opcode GetOpcode(CString &string);
CString GetOpcodeIdentifier(Opcode op);
ESysCallId GetSysCallId(CString &string);
CString GetSysCallIdentifier(ESysCallId id);
bool IsValidSMOpcode(uint32 value);