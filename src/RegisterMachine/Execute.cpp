//Global
#include <cmath>
#include <Windows.h>
//Libs
#include <SJCLib.h>
//Local
#include "Opcodes.h"
#include "ESysCallId.h"
//Namespaces
using namespace SJCLib;

void Run(uint32 oep, const uint32 *pProgram)
{
	bool running;
	int8 immediate8;
	int32 immediate16, immediate24;
	uint32 pc, instruction, opcode, r1, r2, r3, sp;
	float64 regs[NUMBER_OF_REGISTERS];
	float64 stack[1000];
	LARGE_INTEGER freq, t1, t2;
	float64 elapsed;
	
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&t1);
	
	running = true;
	pc = oep;
	sp = 0;

	while(running)
	{
		ASSERT(sp < sizeof(stack)/sizeof(stack[0]));
		
		//decode instruction
		instruction = pProgram[pc++];
		opcode = instruction >> 24;
		//registers
		r1 = (instruction >> 16) & 0xFF;
		r2 = (instruction >> 8) & 0xFF;
		r3 = instruction & 0xFF;
		//immediates
		immediate8 = instruction & 0xFF;
		immediate16 = instruction & 0xFFFF;
		immediate24 = instruction & 0xFFFFFF;
		
		//execute instruction
		switch(opcode)
		{
		case OPCODE_ADD:
			{
				regs[r1] = regs[r2] + regs[r3];
			}
			break;
		case OPCODE_CALL:
			{
				stack[sp++] = pc;
				pc = immediate24;
			}
			break;
		case OPCODE_HLT:
			{
				running = false;
			}
			break;
		case OPCODE_JE:
			{
				if(regs[r1] == regs[r2])
					pc = immediate8;
			}
			break;
		case OPCODE_JLE:
			{
				if(regs[r1] <= regs[r2])
					pc = immediate8;
			}
			break;
		case OPCODE_JZ:
			{
				if(regs[r1] == 0)
					pc = immediate16;
			}
			break;
		case OPCODE_L32:
			{
				regs[r1] = *(float32 *)&pProgram[immediate16];
			}
			break;
		case OPCODE_L64:
			{
				regs[r1] = *(float64 *)&pProgram[immediate16];
			}
			break;
		case OPCODE_MOV:
			{
				regs[r1] = regs[r2];
			}
			break;
		case OPCODE_MUL:
			{
				regs[r1] = regs[r2] * regs[r3];
			}
			break;
		case OPCODE_POP:
			{
				regs[r1] = stack[--sp];
			}
			break;
		case OPCODE_PUSH:
			{
				stack[sp++] = regs[r1];
			}
			break;
		case OPCODE_RET:
			{
				pc = (uint32)stack[--sp];
			}
			break;
		case OPCODE_SUB:
			{
				regs[r1] = regs[r2] - regs[r3];
			}
			break;
		case OPCODE_SYSC:
			{
				switch(immediate24)
				{
				case SYSC_ID_PRINT_NUMBER:
					{
						stdOut << regs[REG_A0] << endl;
					}
					break;
				}
			}
			break;
		default:
			ASSERTMSG(0, CString(opcode));
		}
	}

	QueryPerformanceCounter(&t2);
	
	elapsed = (float64)(t2.QuadPart - t1.QuadPart);
	elapsed *= 1000;
	elapsed /= freq.QuadPart;

	stdOut << endl << "Executed in: " <<  (floor((elapsed + 0.5) * 100) / 100) << " ms" << endl;
}

bool Execute(const CString &refInputFileName)
{
	uint32 oep;
	uint32 *pProgram;
	
	CFileInputStream fis(refInputFileName);

	fis.ReadUInt32(oep);
	pProgram = (uint32 *)MemAlloc(fis.GetRemainingBytes());
	fis.ReadBytes(pProgram, fis.GetRemainingBytes());
	Run(oep, pProgram);
	MemFree(pProgram);
	
	return true;
}