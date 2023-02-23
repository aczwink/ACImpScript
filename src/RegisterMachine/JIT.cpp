//Global
#include <Windows.h>
//Libs
#include <SJCLib.h>
//Namespaces
using namespace SJCLib;

void PutNearReturn(byte *&refpCurrent)
{
	*refpCurrent++ = 0xC3;
}

bool JIT(const CString &refInputFileName)
{
	void *pMem;
	byte *pCurrent;
	void (*pJitMemPointer)();
	
	pMem = VirtualAlloc(NULL, 100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	pCurrent = (byte *)pMem;

	PutNearReturn(pCurrent);
	
	pJitMemPointer = (void (*)())pMem;
	pJitMemPointer();
	
	VirtualFree(pMem, 0, MEM_RELEASE);
	
	return true;
}