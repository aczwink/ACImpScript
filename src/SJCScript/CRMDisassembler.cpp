//Class Header
#include "CRMDisassembler.h"
//SJCLib
#include <SJCLib.h>
//Local
#include "RegisterMachine.h"
//Namespaces
using namespace SJCLib;

//Private Functions
void CRMDisassembler::GetAllLabels()
{
	repeat(this->data.programSize, i)
	{
		switch(this->data.pProgram[i])
		{
		case RM_OPCODE_ADD: //1 value
		case RM_OPCODE_ANEW:
		case RM_OPCODE_CALL:
		case RM_OPCODE_DIVF:
		case RM_OPCODE_I2F:
		case RM_OPCODE_MOV:
		case RM_OPCODE_MOVA:
		case RM_OPCODE_MUL:
		case RM_OPCODE_POP:
		case RM_OPCODE_PUSH:
		case RM_OPCODE_SEL:
		case RM_OPCODE_SUB:
		case RM_OPCODE_SYSC:
			i++;
			break;
		case RM_OPCODE_JMP: //1 value
			{
				int32 jumpOffset;
				
				jumpOffset = this->data.pProgram[i+1];
				this->labels[i + jumpOffset + 2] = this->nextLabelIndex++;
			}
			i++;
			break;
		case RM_OPCODE_LDC: //2 values
			i += 2;
			break;
		case RM_OPCODE_JE: //2 values
		case RM_OPCODE_JL:
		case RM_OPCODE_JLE:
		case RM_OPCODE_JZ:
			{
				int32 jumpOffset;

				jumpOffset = this->data.pProgram[i+2];
				this->labels[i + jumpOffset + 3] = this->nextLabelIndex++;
			}
			i += 2;
			break;
		}
	}
}

void CRMDisassembler::Output2Registers(uint32 offset)
{
	this->file << RM_GetRegisterIdentifier((ERegister)((this->data.pProgram[offset] >> 24) & 0xF))
		<< ", "
		<< RM_GetRegisterIdentifier((ERegister)(this->data.pProgram[offset] & 0xF));
}

void CRMDisassembler::Output3Registers(uint32 offset)
{
	this->file << RM_GetRegisterIdentifier((ERegister)((this->data.pProgram[offset] >> 24) & 0xF))
		<< ", "
		<< RM_GetRegisterIdentifier((ERegister)(this->data.pProgram[offset] & 0xF))
		<< ", "
		<< RM_GetRegisterIdentifier((ERegister)((this->data.pProgram[offset] >> 8) & 0xF));
}

uint32 CRMDisassembler::OutputOpcode(uint32 offset, COFStream &file)
{
	if(!RM_IsValidOpcode(this->data.pProgram[offset]))
		throw CDisassembleException("Invalid Opcode detected.");
	
	file << RM_GetOpcodeIdentifier(this->data.pProgram[offset]);

	switch(this->data.pProgram[offset])
	{
	case RM_OPCODE_I2F:
	case RM_OPCODE_POP:
		file << '\t';
	case RM_OPCODE_PUSH:
		file << '\t';
		this->OutputRegister(offset+1);
		return 2;
	case RM_OPCODE_ADD:
	case RM_OPCODE_MOV:
	case RM_OPCODE_MUL:
	case RM_OPCODE_SUB:
		file << '\t';
	case RM_OPCODE_ANEW:
	case RM_OPCODE_DIVF:
		file << '\t';
		this->Output2Registers(offset+1);
		return 2;
	case RM_OPCODE_MOVA:
	case RM_OPCODE_SEL:
		this->Output3Registers(offset+1);
		return 2;
	case RM_OPCODE_JE:
	case RM_OPCODE_JL:
	case RM_OPCODE_JLE:
		{
			CMapIterator<uint32, uint32> it;
			int32 jumpOffset;

			jumpOffset = this->data.pProgram[offset+2];
			it = this->labels.Find(offset + jumpOffset + 3);

			file << "\t\t";
			this->Output2Registers(offset+1);
			file << ", label" << CString(it.GetValue());
		}
		return 3;
	case RM_OPCODE_CALL:
		file << '\t' << this->data.pCallTable[this->data.pProgram[offset+1]];
		return 2;
	case RM_OPCODE_JMP:
		{
			CMapIterator<uint32, uint32> it;
			int32 jumpOffset;
			
			jumpOffset = this->data.pProgram[offset+1];
			it = this->labels.Find(offset + jumpOffset + 2);
			
			file << "\t\tlabel" << CString(it.GetValue());
		}
		return 2;
	case RM_OPCODE_JZ:
		{
			CMapIterator<uint32, uint32> it;
			int32 jumpOffset;

			jumpOffset = this->data.pProgram[offset+2];
			it = this->labels.Find(offset + jumpOffset + 3);

			file << "\t\t";
			this->OutputRegister(offset+1);
			file << ", label" << CString(it.GetValue());
		}
		return 3;
	case RM_OPCODE_LDC:
		file << "\t\t";
		this->OutputRegister(offset+1);
		file << ", const" << CString(this->data.pProgram[offset+2]);
		return 3;
	case RM_OPCODE_SYSC:
		file << '\t' << GetSysCallIdentifier((ESysCallId)this->data.pProgram[offset+1]);
		return 2;
	}

	return 1;
}

void CRMDisassembler::OutputRegister(uint32 offset)
{
	this->file << RM_GetRegisterIdentifier((ERegister)this->data.pProgram[offset]);
}