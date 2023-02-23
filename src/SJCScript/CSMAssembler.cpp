//Class Header
#include "CSMAssembler.h"

//Constructor
CSMAssembler::CSMAssembler()
{
	this->machineId = VM_STACK;
}

//Private Functions
void CSMAssembler::AssembleOffset()
{
	CAsmLexer tmpLexer;
	Token t;
	int32 offset = 0;

	t = this->lexer.GetNextToken();

	switch(t)
	{
	case TOKEN_SP:
		this->program.Push(1);
		break;
	case TOKEN_BSP:
		this->program.Push(0);
		break;
	default:
		ASSERT(false);
	}

	tmpLexer = this->lexer;
	t = tmpLexer.GetNextToken();
	switch(t)
	{
	case TOKEN_NUMBER:
		this->lexer.GetNextToken();
		offset = tmpLexer.GetCurrentTokenValue().ToInt32();
		break;
	default:
		break;
	}

	this->program.Push(offset);
}

bool CSMAssembler::AssembleOpcode(CString &opcode)
{
	Opcode op;

	op = GetOpcode(opcode);
	ASSERT(op != OPCODE_ILLEGAL);

	switch(op)
	{
	case OPCODE_CALL:
		{
			CString buffer;

			this->program.Push(OPCODE_CALL);
			ASSERT(this->lexer.GetNextToken() == TOKEN_IDENTIFIER);
			buffer = this->lexer.GetCurrentTokenValue();

			if(!this->callIndices.Find(buffer).IsValid())
			{
				this->callIndices[buffer] = this->nextCallIndex++;
			}
			this->program.Push(this->callIndices.Find(buffer).GetValue());
		}
		break;
	case OPCODE_INC:
		this->program.Push(OPCODE_INC);
		this->AssembleOffset();
		break;
	case OPCODE_JE:
	case OPCODE_JL:
	case OPCODE_JLE:
	case OPCODE_JMP:
	case OPCODE_JNE:
	case OPCODE_JZ:
		this->program.Push(op);
		this->AssembleJumpLabel();
		break;
	case OPCODE_LDC:
		ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
		this->program.Push(OPCODE_LDC);
		this->offsetsToInsert[this->program.GetLength()] = lexer.GetCurrentTokenValue();
		this->program.Push(0);
		break;
	case OPCODE_MOV:
	case OPCODE_MOVA:
		this->program.Push(op);
		this->AssembleOffset();
		ASSERT(this->lexer.GetNextToken() == TOKEN_COMMA);
		this->AssembleOffset();
		break;
	case OPCODE_PUSH:
	case OPCODE_SEL:
		this->program.Push(op);
		this->AssembleOffset();
		break;
	case OPCODE_SYSC:
		this->program.Push(OPCODE_SYSC);
		this->AssembleSysCall();
		break;
	case OPCODE_ADD:
	case OPCODE_ADDF:
	case OPCODE_ANEW:
	case OPCODE_DIVF:
	case OPCODE_I2F:
	case OPCODE_MUL:
	case OPCODE_POP:
	case OPCODE_RET:
	case OPCODE_SUB:
		this->program.Push(op);
		break;
	default:
		ASSERT(false);
	}

	return true;
}