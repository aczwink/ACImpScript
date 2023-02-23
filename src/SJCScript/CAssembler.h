#pragma once
//SJCLib
#include <SJCLib.h>
//Local
#include "CAsmLexer.h"
#include "CompiledScript.h"
#include "Machines.h"
#include "Opcodes.h"
//Namespaces
using namespace SJCLib;

class CAssembler abstract
{
private:
	//Variables
	CMap<CString, SScriptFunction> functions;
	CMap<CString, SScriptConstant> constants;
	CMap<CString, uint32> labels;
	CMap<uint32, CString> offsetsToInsertLabels;
	//Functions
	bool AssembleCodeBlock();
	bool AssembleDataBlock();
	bool AssembleProc();
	bool CheckProc(const CString &procName);
	bool CheckProgram();
	void Output(CString &filename);
protected:
	//Variables
	EVirtualMachine machineId;
	CArray<Opcode> program;
	CAsmLexer lexer;
	CMap<uint32, CString> offsetsToInsert;
	CMap<CString, uint32> callIndices;
	uint32 nextCallIndex;
	//Constructor
	CAssembler();
	//Functions
	void AssembleJumpLabel();
	virtual bool AssembleOpcode(CString &opcode) = NULL;
	void AssembleSysCall();
public:
	//Functions
	bool Assemble(CString outputFilename);
	void Init(const char *pBuffer);
};