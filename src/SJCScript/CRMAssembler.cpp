//Class Header
#include "CRMAssembler.h"
//Local
#include "RegisterMachine.h"
//Definitions
#define ASM_REGISTER if(!this->AssembleRegister()) REPORT_ERROR("Failed to assemble register.");
#define REPORT_ERROR(msg) {stdErr << "Error on line " << this->lexer.GetCurrentLineNumber() << ": " << msg << endl; return false;}

//Constructor
CRMAssembler::CRMAssembler()
{
	this->machineId = VM_REGISTER;
}

//Private Functions
void CRMAssembler::Assemble2Registers()
{
	ERegister rd, rs;

	ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
	rd = RM_GetRegister(lexer.GetCurrentTokenValue());

	ASSERT(lexer.GetNextToken() == TOKEN_COMMA);

	ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
	rs = RM_GetRegister(lexer.GetCurrentTokenValue());

	if(rd == REG_ILLEGAL || rs == REG_ILLEGAL)
	{
		ASSERT(false);
	}

	this->program.Push((rd << 24) | rs);
}

void CRMAssembler::Assemble3Registers()
{
	ERegister rd, rs, rs2;

	ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
	rd = RM_GetRegister(lexer.GetCurrentTokenValue());

	ASSERT(lexer.GetNextToken() == TOKEN_COMMA);

	ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
	rs = RM_GetRegister(lexer.GetCurrentTokenValue());

	ASSERT(lexer.GetNextToken() == TOKEN_COMMA);

	ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
	rs2 = RM_GetRegister(lexer.GetCurrentTokenValue());

	if(rd == REG_ILLEGAL || rs == REG_ILLEGAL || rs2 == REG_ILLEGAL)
	{
		ASSERT(false);
	}

	this->program.Push((rd << 24) | (rs2 << 8) | rs);
}

bool CRMAssembler::AssembleOpcode(CString &opcode)
{
	Opcode op;

	op = RM_GetOpcode(opcode);
	ASSERT(op != OPCODE_ILLEGAL);

	switch(op)
	{
	case RM_OPCODE_RET:
		this->program.Push(op);
		break;
	case RM_OPCODE_I2F:
	case RM_OPCODE_POP:
	case RM_OPCODE_PUSH:
		this->program.Push(op);
		ASM_REGISTER;
		break;
	case RM_OPCODE_ADD:
	case RM_OPCODE_ANEW:
	case RM_OPCODE_DIVF:
	case RM_OPCODE_MOV:
	case RM_OPCODE_MUL:
	case RM_OPCODE_SUB:
		this->program.Push(op);
		this->Assemble2Registers();
		break;
	case RM_OPCODE_MOVA:
	case RM_OPCODE_SEL:
		this->program.Push(op);
		this->Assemble3Registers();
		break;
	case RM_OPCODE_JE:
	case RM_OPCODE_JL:
	case RM_OPCODE_JLE:
		this->program.Push(op);
		this->Assemble2Registers();
		ASSERT(lexer.GetNextToken() == TOKEN_COMMA);
		this->AssembleJumpLabel();
		break;
	case RM_OPCODE_CALL:
		{
			CString buffer;

			this->program.Push(op);
			ASSERT(this->lexer.GetNextToken() == TOKEN_IDENTIFIER);
			buffer = this->lexer.GetCurrentTokenValue();

			if(!this->callIndices.Find(buffer).IsValid())
			{
				this->callIndices[buffer] = this->nextCallIndex++;
			}
			this->program.Push(this->callIndices.Find(buffer).GetValue());
		}
		break;
	case RM_OPCODE_JMP:
		this->program.Push(op);
		this->AssembleJumpLabel();
		break;
	case RM_OPCODE_JZ:
		this->program.Push(op);
		ASM_REGISTER;
		ASSERT(lexer.GetNextToken() == TOKEN_COMMA);
		this->AssembleJumpLabel();
		break;
	case RM_OPCODE_LDC:
		this->program.Push(RM_OPCODE_LDC);
		ASM_REGISTER;
		ASSERT(lexer.GetNextToken() == TOKEN_COMMA);
		ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
		this->offsetsToInsert[this->program.GetLength()] = lexer.GetCurrentTokenValue();
		this->program.Push(0);
		break;
	case RM_OPCODE_SYSC:
		this->program.Push(RM_OPCODE_SYSC);
		this->AssembleSysCall();
		break;
	default:
		stdOut << opcode << endl;
		ASSERT(false);
	}

	return true;
}

bool CRMAssembler::AssembleRegister()
{
	ERegister reg;

	if(lexer.GetNextToken() != TOKEN_IDENTIFIER)
		REPORT_ERROR("Expected a register.");
	
	reg = RM_GetRegister(lexer.GetCurrentTokenValue());

	if(reg == REG_ILLEGAL)
		REPORT_ERROR('"' + lexer.GetCurrentTokenValue() + "\" is not a valid register name. See documentation for correct names.");
	
	this->program.Push(reg);

	return true;
}