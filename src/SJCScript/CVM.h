#pragma once
//SJCLib
#include <SJCLib.h>
//Local
#include "CompiledScript.h"
#include "Opcodes.h"
//Namespaces
using namespace SJCLib;

class CVM
{
private:
	UDataWord *pConstants;
	uint32 nFunctions;
	SScriptFunction *pFunctions;
	Opcode *pProgram;
	uint32 *pCallOffsets;
	SScriptData *pScriptData;
public:
	//Constructor
	CVM();
	//Destructor
	~CVM();
	//Functions
	bool LoadModule(CString input);
	void Run();
};