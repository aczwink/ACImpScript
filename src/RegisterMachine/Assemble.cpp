//Libs
#include <SJCLib.h>
//Local
#include "CAsmParser.h"
//Namespaces
using namespace SJCLib;

class CAssembler
{
private:
	//Members
	uint32 oep;
	CArray<byte> program;
	CMap<CString, uint32> labels;
	CMap<uint32, CString> putLabelOffsets8;
	CMap<uint32, CString> putLabelOffsets16;
	CMap<uint32, CString> putLabelOffsets24;
	
	//Methods
	uint32 Assemble(const SInstruction &refInstruction)
	{
		uint32 instruction;
		
		switch(refInstruction.format)
		{
		case IF_OPONLY:
			instruction = refInstruction.opcode << 24;
			break;
		case IF_IMMEDIATE:
			return this->AssembleImmediate(refInstruction);
		case IF_REG:
			return this->AssembleRegister(refInstruction);
		case IF_REG_IMMEDIATE:
			return this->AssembleRegisterImmediate(refInstruction);
		case IF_REG_REG_IMMEDIATE:
			return this->AssembleRegisterRegisterImmediate(refInstruction);
		case IF_REG_REG_REG:
			return this->AssembleRegisterRegisterRegister(refInstruction);
		}

		return instruction;
	}

	uint32 AssembleImmediate(const SInstruction &refInstruction)
	{
		uint32 instruction;

		instruction = refInstruction.opcode << 24;
		
		switch(refInstruction.opcode)
		{
		case OPCODE_CALL:
			this->putLabelOffsets24.Insert(this->program.GetNoOfElements(), refInstruction.label);
			break;
		case OPCODE_SYSC:
			instruction |= refInstruction.immediate;
		}
		
		return instruction;
	}

	uint32 AssembleRegister(const SInstruction &refInstruction)
	{
		uint32 instruction;
		
		instruction = refInstruction.opcode << 24;
		instruction |= refInstruction.regDest << 16;
		
		return instruction;
	}

	uint32 AssembleRegisterImmediate(const SInstruction &refInstruction)
	{
		uint32 instruction, immediate;
		
		immediate = refInstruction.immediate;
		immediate /= 4;
		
		instruction = refInstruction.opcode << 24;
		instruction |= refInstruction.regDest << 16;
		instruction |= (int16)immediate; //the label will be added after
		
		this->putLabelOffsets16.Insert(this->program.GetNoOfElements(), refInstruction.label);

		return instruction;
	}

	uint32 AssembleRegisterRegisterImmediate(const SInstruction &refInstruction)
	{
		uint32 instruction, immediate;

		//check if immediate is word offset
		immediate = refInstruction.immediate / 4;

		instruction = refInstruction.opcode << 24;
		instruction |= refInstruction.regDest << 16;
		instruction |= refInstruction.regSource1 << 8;
		
		this->putLabelOffsets8.Insert(this->program.GetNoOfElements(), refInstruction.label);

		return instruction;
	}

	uint32 AssembleRegisterRegisterRegister(const SInstruction &refInstruction)
	{
		uint32 instruction;
		
		instruction = refInstruction.opcode << 24;
		instruction |= refInstruction.regDest << 16;
		instruction |= refInstruction.regSource1 << 8;
		instruction |= refInstruction.regSource2;
		
		return instruction;
	}

public:
	//Constructor
	CAssembler(CFIFOBuffer &refDataSection, const CMap<CString, uint32> &refDataLabels)
	{
		byte b;
		
		while(refDataSection.GetRemainingBytes())
		{
			b = refDataSection.ReadByte();
			
			this->program.Push(b);
		}

		this->oep = this->program.GetNoOfElements();
		
		this->labels = refDataLabels;
	}

	//Methods
	void Assemble(const CLinkedList<SInstruction> &refInstructions, ASeekableOutputStream &refOutput)
	{
		int8 immediate8;
		int16 immediate16;
		int32 immediate24;
		uint32 instruction;
		
		for(SInstruction const &refInstr : refInstructions)
		{
			if(refInstr.labelOnly)
			{
				this->labels.Insert(refInstr.label, this->program.GetNoOfElements());
				continue;
			}

			instruction = this->Assemble(refInstr);
			
			this->program.Push(instruction & 0xFF);
			this->program.Push((instruction >> 8) & 0xFF);
			this->program.Push((instruction >> 16) & 0xFF);
			this->program.Push((instruction >> 24) & 0xFF);
		}

		//put the labels
		foreach(it, this->putLabelOffsets8)
		{
			immediate8 = this->program[it.GetKey()];
			
			immediate8 += (uint16)this->labels[it.GetValue()];
			immediate8 /= 4;
			
			this->program[it.GetKey()] = immediate8;
		}
		foreach(it, this->putLabelOffsets16)
		{
			immediate16 = this->program[it.GetKey()] | (this->program[it.GetKey()+1] << 8);

			immediate16 += (uint16)this->labels[it.GetValue()];
			immediate16 /= 4;

			this->program[it.GetKey()] = immediate16 & 0xFF;
			this->program[it.GetKey()+1] = immediate16 >> 8;
		}
		foreach(it, this->putLabelOffsets24)
		{
			immediate24 = this->program[it.GetKey()] | (this->program[it.GetKey()+1] << 8) | (this->program[it.GetKey()+2] << 16);

			immediate24 += (int32)this->labels[it.GetValue()];
			immediate24 /= 4;
			
			this->program[it.GetKey()] = immediate24 & 0xFF;
			this->program[it.GetKey()+1] = (immediate24 >> 8) & 0xFF;
			this->program[it.GetKey()+2] = immediate24 >> 16;
		}

		//write prog
		refOutput.WriteUInt32(oep / 4);
		foreach(it, this->program)
		{
			refOutput.WriteByte(*it);
		}
	}
};

bool Assemble(const CString &refInputFileName)
{
	CFileOutputStream output;
	
	CAsmParser &refParser = CAsmParser(refInputFileName);
	
	refParser.ParseBlocks();

	if(!output.Open(refInputFileName + ".sjccs"))
	{
		ASSERT(0);
	}
	
	CAssembler &refAsm = CAssembler(refParser.GetDataSection(), refParser.GetDataLabels());

	refAsm.Assemble(refParser.GetInstructions(), output);
	
	return true;
}