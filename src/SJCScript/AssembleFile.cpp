//SJCLib
#include <SJCLibFile.h>
#include <SJCLibString.h>
//Global
#include <iostream>
//Local
#include "CAssembler.h"
#include "CRMAssembler.h"
#include "CSMAssembler.h"
#include "Machines.h"
//Namespaces
using namespace SJCLibFile;
using namespace SJCLibString;

bool AssembleFile(CString &filename, EVirtualMachine vm)
{
	CAssembler *pAssembler;
	CBIFStream file;
	char *pBuffer;
	bool ok;

	if(!file.Open(filename))
	{
		return false;
	}
	pBuffer = (char *)malloc(file.GetFileSize()+1);
	file.ReadBytes(pBuffer, file.GetFileSize());
	pBuffer[file.GetFileSize()] = '\0';

	switch(vm)
	{
	case VM_STACK:
		pAssembler = new CSMAssembler;
		break;
	case VM_REGISTER:
		pAssembler = new CRMAssembler;
		break;
	}
	pAssembler->Init(pBuffer);
	ok = pAssembler->Assemble(GetFilePath(filename) + "\\" + GetFileName(filename) + '.' + COMPILEDSCRIPT_FILEEXT);
	free(pBuffer);
	delete pAssembler;

	if(!ok)
	{
		stdErr << "Assembling failed." << endl;
		return false;
	}

	return ok;
}