//Class Header
#include "CAsmParser.h"
//Local
#include "AsmLexerTable.h"
#include "ESysCallId.h"

//Constructor
CAsmParser::CAsmParser(const CString &refInputFileName)
{
	uint32 size;
	
	CFileInputStream fis(refInputFileName);

	size = fis.GetRemainingBytes();
	this->pScript = (byte *)MemAlloc(size+1);
	this->pScript[size] = 0;
	fis.ReadBytes(this->pScript, size);
	
	this->lexer.SetInput(this->pScript);
	this->lookahead = this->lexer.GetNextToken();
}

//Destructor
CAsmParser::~CAsmParser()
{
	MemFree(this->pScript);
}

//Private methods
uint8 CAsmParser::ExpectRegister()
{
	char type;
	uint8 number;
	CString reg;
	
	reg = this->lexer.GetTokenValue();
	
	type = reg[0];
	number = (uint8)reg.SubString(1, reg.GetLength()-1).ToUInt64();

	this->Expect(TOKEN_REGISTER);

	if(type != '$')
	{
		ASSERT(number < 32);
	}

	switch(type)
	{
	case 'a':
		return number + 32;
	case 'r':
		return number;
	case 's':
		return number + 2 * 32;
	case 't':
		return number + 3 * 32;
	default:
		ASSERT(0);
	}

	return number;
}

uint32 CAsmParser::GetSysCallId()
{
	CString sysCallId;
	
	sysCallId = this->lexer.GetTokenValue();
	
	this->Expect(TOKEN_IDENTIFIER);

	if(sysCallId == "SYSC_PRINT_NUMBER")
		return SYSC_ID_PRINT_NUMBER;
	
	ASSERT(0);
	return -1;
}

bool CAsmParser::InInstructionSet(const SInstructionNameToOp *pMap, uint8 nOps, EOpcode &refOpcode)
{
	const CString &refInstr = this->lexer.GetTokenValue();
	
	for(uint32 i = 0; i < nOps; i++)
	{
		if(refInstr == pMap[i].name)
		{
			refOpcode = pMap[i].op;
			return true;
		}
	}
	
	return false;
}

bool CAsmParser::IsIdentifierInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"call", OPCODE_CALL},
		{"sysc", OPCODE_SYSC},
	};
	
	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

bool CAsmParser::IsNoOperandInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"hlt", OPCODE_HLT},
		{"ret", OPCODE_RET},
	};
	
	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

bool CAsmParser::IsRegInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"pop", OPCODE_POP},
		{"push", OPCODE_PUSH}
	};

	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

bool CAsmParser::IsRegIdentifierInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"jz", OPCODE_JZ},
	};

	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

bool CAsmParser::IsRegAddressInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"l32", OPCODE_L32},
		{"l64", OPCODE_L64}
	};

	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

bool CAsmParser::IsRegRegInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"mov", OPCODE_MOV},
	};

	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

bool CAsmParser::IsRegRegIdentifierInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"je", OPCODE_JE},
		{"jle", OPCODE_JLE},
	};

	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

bool CAsmParser::IsRegRegRegInstruction(EOpcode &refOpcode)
{
	SInstructionNameToOp instructions[] =
	{
		{"add", OPCODE_ADD},
		{"mul", OPCODE_MUL},
		{"sub", OPCODE_SUB},
	};

	return this->InInstructionSet(instructions, sizeof(instructions) / sizeof(instructions[0]), refOpcode);
}

/*
ADDRESS -> number(label)
*/
void CAsmParser::ParseAddress(SInstruction &refInstruction)
{
	refInstruction.immediate = (uint32)this->lexer.GetTokenValue().ToFloat();

	this->Expect(TOKEN_NUMBER);
	this->Expect(TOKEN_PARENOPEN);
	
	refInstruction.label = this->lexer.GetTokenValue();
	
	this->Expect(TOKEN_IDENTIFIER);
	this->Expect(TOKEN_PARENCLOSE);
}

/*
CODEBLOCK -> .code CODEENTRIES;
*/
void CAsmParser::ParseCodeBlock()
{
	this->Expect(TOKEN_KEYWORD_CODEBLOCK);
	this->ParseCodeEntries();
}

/*
CODEENTRIES -> INSTRUCTION;
CODEENTRIES -> DATAENTRY;
CODEENTRIES -> CODEENTRIES CODEENTRIES;
*/
void CAsmParser::ParseCodeEntries()
{
	bool ok;

	ok = true;
	
	while(ok)
	{
		switch(this->lookahead)
		{
		case TOKEN_IDENTIFIER:
			ok = this->ParseInstruction();
			break;
		case TOKEN_LABEL:
			{
				SInstruction instr;

				instr.labelOnly = true;
				instr.label = this->lexer.GetTokenValue();

				this->instructions.InsertTail(instr);

				this->NextLookahead();
			}
			break;
		case TOKEN_END:
			ok = false;
			break;
		default:
			ASSERT(0);
		}
	}
}

void CAsmParser::ParseDataBlock()
{
	if(!this->Accept(TOKEN_KEYWORD_DATABLOCK))
		return;
	
	this->ParseDataEntries();
}

/*
DATA -> .float32 DATA;
DATA -> .float64 DATA;
DATA -> number;
DATA -> DATA, DATA;
*/
void CAsmParser::ParseData()
{
	switch(this->lookahead)
	{
	case TOKEN_KEYWORD_FLOAT32:
		{
			this->currentType = TYPE_FLOAT32;
			
			this->NextLookahead();
			this->ParseData();
		}
		break;
	case TOKEN_KEYWORD_FLOAT64:
		{
			this->currentType = TYPE_FLOAT64;
			
			this->NextLookahead();
			this->ParseData();
		}
		break;
	case TOKEN_NUMBER:
		{
			CString number;
			
			number = this->lexer.GetTokenValue();
			
			this->NextLookahead();

			switch(this->currentType)
			{
			case TYPE_FLOAT32:
				{
					this->dataSection.WriteFloat32((float32)number.ToFloat());
				}
				break;
			case TYPE_FLOAT64:
				{
					this->dataSection.WriteFloat64(number.ToFloat());
				}
				break;
			}
		}
		break;
	default:
		this->ParseData();
		this->Expect(TOKEN_COMMA);
		this->ParseData();
	}
}

/*
DATAENTRIES -> DATAENTRY;
DATAENTRIES -> DATAENTRY DATAENTRIES;
*/
void CAsmParser::ParseDataEntries()
{
	while(this->lookahead == TOKEN_LABEL)
	{
		this->ParseDataEntry();
	}
}

/*
DATAENTRY -> label: DATA;
*/
void CAsmParser::ParseDataEntry()
{
	CString label;
	
	label = this->lexer.GetTokenValue();
	
	this->Expect(TOKEN_LABEL);
	
	this->dataLabels.Insert(label, this->dataSection.GetRemainingBytes());
	this->ParseData();
}

/*
INSTRUCTION -> identifier;
INSTRUCTION -> identifier register, ADDRESS;
INSTRUCTION -> identifier identifier;
*/
bool CAsmParser::ParseInstruction()
{
	SInstruction instruction;

	instruction.labelOnly = false;
	
	if(this->IsIdentifierInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_IMMEDIATE;
		if(instruction.opcode == OPCODE_SYSC)
		{
			instruction.immediate = this->GetSysCallId();
		}
		else
		{
			instruction.label = this->lexer.GetTokenValue();
			
			this->Expect(TOKEN_IDENTIFIER);
		}
		
		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else if(this->IsNoOperandInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_OPONLY;

		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else if(this->IsRegInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_REG;
		instruction.regDest = this->ExpectRegister();

		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else if(this->IsRegIdentifierInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_REG_IMMEDIATE;
		instruction.immediate = 0;
		instruction.regDest = this->ExpectRegister();
		this->Expect(TOKEN_COMMA);
		
		instruction.label = this->lexer.GetTokenValue();
		this->Expect(TOKEN_IDENTIFIER);

		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else if(this->IsRegAddressInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_REG_IMMEDIATE;
		instruction.regDest = this->ExpectRegister();

		this->Expect(TOKEN_COMMA);
		this->ParseAddress(instruction);

		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else if(this->IsRegRegInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_REG_REG_IMMEDIATE;
		
		instruction.regDest = this->ExpectRegister();
		this->Expect(TOKEN_COMMA);
		
		instruction.regSource1 = this->ExpectRegister();
		
		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else if(this->IsRegRegIdentifierInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_REG_REG_IMMEDIATE;
		
		instruction.regDest = this->ExpectRegister();
		this->Expect(TOKEN_COMMA);
		
		instruction.regSource1 = this->ExpectRegister();
		this->Expect(TOKEN_COMMA);
		
		instruction.label = this->lexer.GetTokenValue();
		this->Expect(TOKEN_IDENTIFIER);
		
		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else if(this->IsRegRegRegInstruction(instruction.opcode))
	{
		this->NextLookahead();
		
		instruction.format = IF_REG_REG_REG;
		
		instruction.regDest = this->ExpectRegister();
		this->Expect(TOKEN_COMMA);
		
		instruction.regSource1 = this->ExpectRegister();
		this->Expect(TOKEN_COMMA);

		instruction.regSource2 = this->ExpectRegister();
		
		this->instructions.InsertTail(instruction);
		
		return true;
	}
	else
	{
		ASSERTMSG(0, this->lexer.GetTokenValue());
	}
	
	return false;
}

//Public methods
void CAsmParser::ParseBlocks()
{
	this->ParseDataBlock();
	this->ParseCodeBlock();
}