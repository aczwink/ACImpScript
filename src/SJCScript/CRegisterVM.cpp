//Class Header
#include "CRegisterVM.h"
//Global
#include <Windows.h>
//Local
#include "RegisterMachine.h"
//Namespaces
using namespace SJCLib;
//Global Variables
extern bool g_DebugMode;
//Definitions
#define RD(input) ((input >> 24) & 0xF) //destination register
#define RS(input) (input & 0xF) //source register
#define RS2(input) ((input >> 8) & 0xF) //second source register

//Constructor
CRegisterVM::CRegisterVM()
{
	this->nFunctions = 0;
	this->pCallOffsets = NULL;
	this->pConstants = NULL;
	this->pFunctions = NULL;
	this->pProgram = NULL;
	this->pScriptData = NULL;
}

//Destructor
CRegisterVM::~CRegisterVM()
{
	memfreesafe(this->pCallOffsets);
	memfreesafe(this->pConstants);
	//memfree(this->pFunctions);
	//memfree(this->pProgram);
	if(this->pScriptData)
	{
		ReleaseCompiledScript(this->pScriptData);
		memfree(this->pScriptData);
	}
}

//Public Functions
bool CRegisterVM::LoadModule(CString input)
{
	this->pScriptData = (SScriptData *)malloc(sizeof(*this->pScriptData));
	MemZero(this->pScriptData, sizeof(*this->pScriptData));

	if(!ReadCompiledScript(input, this->pScriptData))
	{
		stdErr << "Error: Failed to load file '" << input << "'. Is this a compiled script file?" << endl;
		return false;
	}
	if(this->pScriptData->machineId != VM_REGISTER)
	{
		stdErr << "Error: '" << input << "' is not compatible with this machine." << endl;
		return false;
	}

	this->nFunctions = this->pScriptData->nFunctions;
	this->pFunctions = this->pScriptData->pFunctions;
	this->pProgram = this->pScriptData->pProgram;

	//Constants
	this->pConstants = (UDataWord *)malloc(this->pScriptData->nConstants * sizeof(*this->pConstants));
	MemZero(this->pConstants, this->pScriptData->nConstants * sizeof(*this->pConstants));
	repeat(this->pScriptData->nConstants, i)
	{
		switch(this->pScriptData->pConstants[i].type)
		{
		case SCRIPTVALUE_FLOAT:
			this->pConstants[i].fValue = this->pScriptData->pConstants[i].fValue;
			break;
		case SCRIPTVALUE_INT:
			this->pConstants[i].intValue = this->pScriptData->pConstants[i].intValue;
			break;
		case SCRIPTVALUE_STRING:
			this->pConstants[i].pStrValue = this->pScriptData->pConstants[i].pStrValue;
			break;
		default:
			ASSERT(false);
		}
	}

	//Function Call Offsets
	this->pCallOffsets = (uint32 *)malloc(this->pScriptData->nCallTableEntries * sizeof(*this->pCallOffsets));
	repeat(this->pScriptData->nCallTableEntries, i)
	{
		repeat(this->pScriptData->nFunctions, j)
		{
			if(*this->pScriptData->pFunctions[j].pName == this->pScriptData->pCallTable[i])
			{
				this->pCallOffsets[i] = this->pScriptData->pFunctions[j].offset;
			}
		}
	}
	
	return true;
}

void CRegisterVM::Run()
{
	Opcode *pc; //program counter
	Opcode *pEnd; //end of program
	UDataWord r[REGVM_NREGISTERS]; //Registers
	CArray<UDataWord> stack;
	CArray<Opcode *> callStack;
	CArray<UDataWord *> allocatedArrays;
	uint32 startupTime, endTime;

	//---Init
	//Set program counter to point to start of main
	repeat(this->nFunctions, i)
	{
		if(*this->pFunctions[i].pName == COMPILEDSCRIPT_ENTRYPOINT)
		{
			pc = &this->pProgram[this->pFunctions[i].offset];
			pEnd = pc + this->pFunctions[i].nOpcodes;
			break;
		}
	}
	MemZero(&r, sizeof(r));

	startupTime = GetTickCount();

	//Run machine
	while(pc != pEnd)
	{
		switch(*pc++)
		{
		case RM_OPCODE_ADD:
			r[RD(*pc++)].intValue += r[RS(*pc)].intValue;
			break;
		case RM_OPCODE_ANEW:
			allocatedArrays.Push(new UDataWord[r[RS(*pc)].intValue]);
			r[RD(*pc++)].pArray = allocatedArrays[allocatedArrays.GetLength()-1];
			break;
		case RM_OPCODE_CALL:
			callStack.Push(pc+1);
			pc = this->pProgram + this->pCallOffsets[*pc];
			break;
		case RM_OPCODE_DIVF:
			r[RD(*pc++)].fValue /= r[RS(*pc)].fValue;
			break;
		case RM_OPCODE_I2F:
			r[RS(*pc++)].fValue = (float)r[RS(*pc)].intValue;
			break;
		case RM_OPCODE_JE:
			if(r[RD(*pc++)].intValue == r[RS(*pc)].intValue)
				pc += *pc;
			pc++;
			break;
		case RM_OPCODE_JL:
			if(r[RD(*pc++)].intValue < r[RS(*pc)].intValue)
				pc += *pc;
			pc++;
			break;
		case RM_OPCODE_JLE:
			if(r[RD(*pc++)].intValue <= r[RS(*pc)].intValue)
				pc += *pc;
			pc++;
			break;
		case RM_OPCODE_JMP:
			pc += int32(*pc++);
			break;
		case RM_OPCODE_JZ:
			if(!r[*pc++].intValue)
				pc += *pc;
			pc++;
			break;
		case RM_OPCODE_LDC:
			r[*pc] = this->pConstants[*(pc+1)];
			pc += 2;
			break;
		case RM_OPCODE_MOV:
			r[RD(*pc++)] = r[RS(*pc)];
			break;
		case RM_OPCODE_MOVA:
			r[RD(*pc)].pArray[r[RS(*pc++)].intValue] = r[RS2(*pc)];
			break;
		case RM_OPCODE_MUL:
			r[RD(*pc++)].intValue *= r[RS(*pc)].intValue;
			break;
		case RM_OPCODE_POP:
			r[*pc++] = stack.Pop();
			break;
		case RM_OPCODE_PUSH:
			stack.Push(r[*pc++]);
			break;
		case RM_OPCODE_RET:
			pc = callStack.Pop();
			break;
		case RM_OPCODE_SEL:
			r[RD(*pc++)] = r[RS(*pc)].pArray[r[RS2(*pc)].intValue];
			break;
		case RM_OPCODE_SUB:
			r[RD(*pc++)].intValue -= r[RS(*pc)].intValue;
			break;
		case RM_OPCODE_SYSC:
			switch(*pc++)
			{
			case SYSCALL_PRINTI:
				stdOut << r[REG_A0].intValue;
				break;
			case SYSCALL_PRINTF:
				stdOut << r[REG_A0].fValue;
				break;
			case SYSCALL_PRINTS:
				stdOut << *r[REG_A0].pStrValue;
				break;
			case SYSCALL_RAND:
				r[REG_R0].intValue = rand();
				break;
			}
			break;
		default:
			ASSERT(false);
		}
	}

	endTime = GetTickCount();
	stdOut << endl;
	if(g_DebugMode)
	{
		stdOut << endl << "Debug Output:" << endl
			<< "-Execution took " << endTime-startupTime << " ms" << endl;
		if(stack.GetLength())
		{
			stdOut << "-Stack is not empty, there are " << stack.GetLength() << " elements on it." << endl;
		}
		if(!allocatedArrays.IsEmpty())
		{
			stdOut << "Freeing Heap..." << endl;
			for(int32 i = allocatedArrays.GetLength()-1; i >= 0; i--)
			{
				delete [] allocatedArrays[i];
			}
		}
	}
}