//SJCLib
#include <SJCLib.h>
//Local
#include "CDisassembler.h"
#include "CRMDisassembler.h"
#include "CSMDisassembler.h"
//Namespaces
using namespace SJCLib;

bool DisassembleFile(CString filename, EVirtualMachine vm)
{
	CDisassembler *pDasm;
	bool ok;

	switch(vm)
	{
	case VM_STACK:
		pDasm = new CSMDisassembler;
		break;
	case VM_REGISTER:
		pDasm = new CRMDisassembler;
		break;
	}

	if(!pDasm->Init(vm, filename))
	{
		stdErr << "Disassembling failed." << endl;
		delete pDasm;
		return false;
	}
	
	try
	{
		ok = pDasm->Output(GetFilePath(filename) + "\\" + GetFileName(filename) + ".sjcasm");
	}
	catch(CDisassembleException e)
	{
		stdErr << e.message << endl;
		ok = false;
	}

	delete pDasm;	
	return ok;
}