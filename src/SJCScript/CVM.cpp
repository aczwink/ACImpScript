//Class Header
#include "CVM.h"
//Global
#include <ctime>
#include <Windows.h>
//SJCLib
#include <SJCLib.h>
//Namespaces
using namespace SJCLib;
//Definitions
#define GETSTACKOFFSET(var) { var = *pc++; var ? var = stack.GetLength()-1+(int32)*pc++ : var = bsp+(int32)*pc++; }
#define GETSTRING(var) { var = ""; do{ var += *pc++; }while(*pc); pc++; }
//Global Variables
extern bool g_DebugMode;

//Constructor
CVM::CVM()
{
	this->nFunctions = 0;
	this->pCallOffsets = NULL;
	this->pConstants = NULL;
	this->pFunctions = NULL;
	this->pProgram = NULL;
	this->pScriptData = NULL;
}

//Destructor
CVM::~CVM()
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
bool CVM::LoadModule(CString input)
{
	this->pScriptData = (SScriptData *)malloc(sizeof(*this->pScriptData));
	MemZero(this->pScriptData, sizeof(*this->pScriptData));

	if(!ReadCompiledScript(input, this->pScriptData))
	{
		stdErr << "Error: Failed to load file '" << input << "'. Is this a compiled script file?" << endl;
		return false;
	}
	if(this->pScriptData->machineId != VM_STACK)
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

void CVM::Run()
{
	register Opcode *pc; //program counter
	register int32 r1, r2;
	UDataWord accu; //the accumulator
	UDataWord si; //may be only used for integer ops
	UDataWord sf; //may be only used for floating-point ops
	CArray<UDataWord> stack; //stack where the values lay on, every string op must set pStrValue to NULL after it has finished
	register uint32 bsp; //base stack pointer
	CArray<Opcode *> callStack; //saves program counter and bsp before a call
	Opcode *pProgram; //the program
	UDataWord *pConstants; //constant pool
	uint32 startupTime, endTime;
	CArray<UDataWord *> allocatedArrays;
	
	//Init
	r1 = 0;
	r2 = 0;
	bsp = 0;
	MemZero(&accu, sizeof(accu));
	MemZero(&si, sizeof(si));
	MemZero(&sf, sizeof(sf));
	stack.EnsureCapacity(1000);
	callStack.EnsureCapacity(1000);
	pProgram = this->pProgram;
	pConstants = this->pConstants;

	//Set program counter to point to start of main
	callStack.Push(NULL);
	callStack.Push(0);
	repeat(this->nFunctions, i)
	{
		if(*this->pFunctions[i].pName == COMPILEDSCRIPT_ENTRYPOINT)
		{
			pc = &this->pProgram[this->pFunctions[i].offset];
			break;
		}
	}
	
	startupTime = GetTickCount();
	srand((unsigned int)time(NULL)); //Init random number generator
	
	while(pc)
	{
		switch(*pc++)
		{
		case OPCODE_ADD:
			si.intValue = stack.Pop().intValue + stack.Pop().intValue;
			stack.Push(si);
			break;
		case OPCODE_ADDF:
			sf.fValue = stack.Pop().fValue + stack.Pop().fValue;
			stack.Push(sf);
			break;
		case OPCODE_ANEW:
			accu.pArray = new UDataWord[stack.Pop().intValue];
			allocatedArrays.Push(accu.pArray);
			stack.Push(accu);
			break;
		case OPCODE_CALL:
			callStack.Push(pc+1);
			callStack.Push((Opcode *)bsp);
			bsp = stack.GetLength()-1;
			pc = pProgram + this->pCallOffsets[*pc];
			break;
		case OPCODE_DIVF:
			sf.fValue = stack.Pop().fValue / stack.Pop().fValue;
			stack.Push(sf);
			break;
		case OPCODE_I2F:
			sf.fValue = (float)stack.Pop().intValue;
			stack.Push(sf);
			break;
		case OPCODE_INC:
			GETSTACKOFFSET(r1);
			stack[r1].intValue++;
			break;
		case OPCODE_JE:
			if(stack.Pop().intValue == stack.Pop().intValue)
				pc += *pc;
			pc++;
			break;
		case OPCODE_JL:
			if(stack.Pop().intValue < stack.Pop().intValue)
				pc += *pc;
			pc++;
			break;
		case OPCODE_JLE:
			if(stack.Pop().intValue <= stack.Pop().intValue)
				pc += *pc;
			pc++;
			break;
		case OPCODE_JMP:
			pc += int32(*pc++);
			break;
		case OPCODE_JNE:
			if(stack.Pop().intValue != stack.Pop().intValue)
				pc += *pc;
			pc++;
			break;
		case OPCODE_JZ:
			if(stack.Pop().intValue == 0)
				pc += *pc;
			pc++;
			break;
		case OPCODE_LDC:
			stack.Push(pConstants[*pc++]);
			break;
		case OPCODE_MOV:
			GETSTACKOFFSET(r1);
			GETSTACKOFFSET(r2);
			stack[r1] = stack[r2];
			break;
		case OPCODE_MOVA:
			GETSTACKOFFSET(r1);
			GETSTACKOFFSET(r2);
			stack[r1].pArray[stack.Pop().intValue] = stack[r2];
			break;
		case OPCODE_MUL:
			si.intValue = stack.Pop().intValue * stack.Pop().intValue;
			stack.Push(si);
			break;
		case OPCODE_POP:
			stack.Pop();
			break;
		case OPCODE_PUSH:
			GETSTACKOFFSET(r1);
			stack.Push(stack[r1]);
			break;
		case OPCODE_RET:
			bsp = (uint32)callStack.Pop();
			pc = callStack.Pop();
			break;
		case OPCODE_SEL:
			GETSTACKOFFSET(r1);
			stack.Push(stack[r1].pArray[stack.Pop().intValue]);
			break;
		case OPCODE_SUB:
			si.intValue = stack.Pop().intValue - stack.Pop().intValue;
			stack.Push(si);
			break;
		case OPCODE_SYSC:
			switch(*pc++)
			{
			case SYSCALL_PRINTI:
				stdOut << stack.Pop().intValue;
				break;
			case SYSCALL_PRINTF:
				stdOut << stack.Pop().fValue;
				break;
			case SYSCALL_PRINTS:
				stdOut << *stack.Pop().pStrValue;
				break;
			case SYSCALL_RAND:
				accu.intValue = rand();
				stack.Push(accu);
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