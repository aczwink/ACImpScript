#pragma once
//SJCLib
#include <SJCLib.h>
//Local
#include "CVM.h"
#include "Opcodes.h"
//Namespaces
using namespace SJCLib;

class CDisassembleException
{
public:
	//Variables
	CString message;
	//Constructor
	CDisassembleException(CString msg)
	{
		this->message = msg;
	}
};

class CDisassembler abstract
{
private:
	//Functions
	virtual void GetAllLabels() = NULL;
	virtual uint32 OutputOpcode(uint32 offset, COFStream &file) = NULL;
	void OutputProcedure(SScriptFunction *pFn, COFStream &file);
protected:
	//Variables
	COFStream file;
	SScriptData data;
	CMap<uint32, uint32> labels;
	uint32 nextLabelIndex;
public:
	//Constructor
	CDisassembler();
	//Destructor
	~CDisassembler();
	//Functions
	bool Init(EVirtualMachine machineId, CString filename);
	bool Output(CString filename);
};