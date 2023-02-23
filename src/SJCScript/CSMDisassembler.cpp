//Class Header
#include "CSMDisassembler.h"
//SJCLib
#include <SJCLib.h>
//Local
#include "CAsmLexer.h"
//Namespaces
using namespace SJCLib;

//Private Functions
void CSMDisassembler::GetAllLabels()
{
	repeat(this->data.programSize, i)
	{
		switch(this->data.pProgram[i])
		{
		case OPCODE_CALL: //1 value
		case OPCODE_LDC:
			i++;
			break;
		case OPCODE_INC: //2 values
		case OPCODE_PUSH:
		case OPCODE_SEL:
		case OPCODE_SYSC:
			i += 2;
			break;
		case OPCODE_MOV: //4 values
		case OPCODE_MOVA:
			i += 4;
			break;
		case OPCODE_JE:
		case OPCODE_JL:
		case OPCODE_JLE:
		case OPCODE_JMP:
		case OPCODE_JNE:
		case OPCODE_JZ:
			{
				int32 jumpOffset;

				jumpOffset = this->data.pProgram[++i];
				this->labels[i + jumpOffset + 1] = this->nextLabelIndex++;
			}
			break;
		}
	}
}

void CSMDisassembler::OutputOffset(uint32 offset, COFStream &file)
{
	int32 stackOffset;

	if(this->data.pProgram[offset])
	{
		file << KEYWORD_SP;
	}
	else
	{
		file << KEYWORD_BSP;
	}

	stackOffset = this->data.pProgram[offset+1];

	if(stackOffset == 0)
		return;
	if(stackOffset > 0)
		file << '+';
	file << CString(stackOffset);
}

uint32 CSMDisassembler::OutputOpcode(uint32 offset, COFStream &file)
{
	if(!IsValidSMOpcode(this->data.pProgram[offset]))
		throw CDisassembleException("Invalid Opcode detected.");
	
	file << GetOpcodeIdentifier(this->data.pProgram[offset]);

	switch(this->data.pProgram[offset])
	{
	case OPCODE_CALL:
		file << '\t' << this->data.pCallTable[this->data.pProgram[offset+1]];
		return 2;
	case OPCODE_JE:
	case OPCODE_JL:
	case OPCODE_JLE:
	case OPCODE_JMP:
	case OPCODE_JNE:
	case OPCODE_JZ:
		{
			CMapIterator<uint32, uint32> it;
			int32 jumpOffset;
			
			jumpOffset = this->data.pProgram[offset+1];
			it = this->labels.Find(offset + jumpOffset + 2);

			file << "\t\tlabel" << CString(it.GetValue());
		}
		return 2;
	case OPCODE_LDC:
		file << "\t\tconst" << CString(this->data.pProgram[offset+1]);
		return 2;
	case OPCODE_MOV:
	case OPCODE_MOVA:
		file << "\t\t";
		this->OutputOffset(offset+1, file);
		file << ", ";
		this->OutputOffset(offset+3, file);
		return 5;
	case OPCODE_INC:
		file << "\t\t";
		this->OutputOffset(offset+1, file);
		return 3;
	case OPCODE_PUSH:
	case OPCODE_SEL:
		file << '\t';
		this->OutputOffset(offset+1, file);
		return 3;
	case OPCODE_SYSC:
		file << '\t' << GetSysCallIdentifier((ESysCallId)this->data.pProgram[offset+1]);
		return 2;
	}

	return 1;
}