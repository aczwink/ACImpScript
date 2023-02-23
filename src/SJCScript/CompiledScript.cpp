//Main Header
#include "CompiledScript.h"
//Global
#include <iostream>
//SJCLib
#include <SJCLib.h>
//Local
#include "CVM.h"
//Namespaces
using namespace SJCLib;

bool ReadCompiledScript(CString filename, SScriptData *pData)
{
	CBIFStream file;
	char signature[COMPILEDSCRIPT_SIGNATURELENGTH];
	
	if(!file.Open(filename))
		return false;
	
	file.ReadBytes(&signature, sizeof(signature));
	if((CString)COMPILEDSCRIPT_SIGNATURE == CString(signature, sizeof(signature)))
	{
		file.ReadBytes(&pData->machineId, sizeof(pData->machineId));
		file.ReadUInt32(&pData->nFunctions);
		file.ReadUInt32(&pData->programSize);
		file.ReadUInt32(&pData->nConstants);
		file.ReadUInt32(&pData->nCallTableEntries);
		
		//Functions
		pData->pFunctions = (SScriptFunction *)malloc(pData->nFunctions * sizeof(*pData->pFunctions));
		repeat(pData->nFunctions, i)
		{
			pData->pFunctions[i].pName = new CString;
			*pData->pFunctions[i].pName = file.ReadString();
			file.ReadUInt32(&pData->pFunctions[i].offset);
			file.ReadUInt32(&pData->pFunctions[i].nOpcodes);
		}
		
		//Program
		pData->pProgram = (Opcode *)malloc(pData->programSize * sizeof(*pData->pProgram));
		file.ReadBytes(pData->pProgram, pData->programSize * sizeof(*pData->pProgram));
		
		//Constants
		pData->pConstants = (SScriptConstant *)malloc(pData->nConstants * sizeof(*pData->pConstants));
		repeat(pData->nConstants, i)
		{
			file.ReadBytes(&pData->pConstants[i].type, sizeof(pData->pConstants[i].type));
			switch(pData->pConstants[i].type)
			{
			case SCRIPTVALUE_FLOAT:
				file.ReadFloat(&pData->pConstants[i].fValue);
				pData->pConstants[i].pStrValue = NULL;
				break;
			case SCRIPTVALUE_INT:
				file.ReadInt32(&pData->pConstants[i].intValue);
				pData->pConstants[i].pStrValue = NULL;
				break;
			case SCRIPTVALUE_STRING:
				pData->pConstants[i].intValue = 0;
				pData->pConstants[i].pStrValue = new CString;
				*pData->pConstants[i].pStrValue = file.ReadString();
				break;
			}
		}

		//Calltable
		pData->pCallTable = new CString[pData->nCallTableEntries];
		repeat(pData->nCallTableEntries, i)
		{
			pData->pCallTable[i] = file.ReadString();
		}
		
		if(file.GetRemainingBytes() == 0)
		{
			return true;
		}
	}

	return false;
}

void ReleaseCompiledScript(SScriptData *pData)
{
	repeat(pData->nFunctions, i)
	{
		HeapDelete(pData->pFunctions[i].pName);
	}
	memfree(pData->pFunctions);

	memfree(pData->pProgram);

	repeat(pData->nConstants, i)
	{
		if(pData->pConstants[i].type == SCRIPTVALUE_STRING)
		{
			HeapDelete(pData->pConstants[i].pStrValue);
		}
	}
	memfree(pData->pConstants);

	HeapDeleteArray(pData->pCallTable);
}

bool WriteCompiledScript(CString filename, SScriptData *pData)
{
	COFStream file;
	
	if(!file.Open(filename))
	{
		return false;
	}
	
	//Header
	file.WriteBytes(COMPILEDSCRIPT_SIGNATURE, COMPILEDSCRIPT_SIGNATURELENGTH);
	file.WriteBytes(&pData->machineId, sizeof(pData->machineId));
	file.WriteUInt32(pData->nFunctions);
	file.WriteUInt32(pData->programSize);
	file.WriteUInt32(pData->nConstants);
	file.WriteUInt32(pData->nCallTableEntries);

	//Functions
	repeat(pData->nFunctions, i)
	{
		file.WriteString(*pData->pFunctions[i].pName);
		file.WriteByte(0);
		file.WriteUInt32(pData->pFunctions[i].offset);
		file.WriteUInt32(pData->pFunctions[i].nOpcodes);
	}

	//Program
	file.WriteBytes(pData->pProgram, pData->programSize * sizeof(*pData->pProgram));
	
	//Constants
	repeat(pData->nConstants, i)
	{
		file.WriteBytes(&pData->pConstants[i].type, sizeof(pData->pConstants[i].type));
		switch(pData->pConstants[i].type)
		{
		case SCRIPTVALUE_FLOAT:
			file.WriteFloat(pData->pConstants[i].fValue);
			break;
		case SCRIPTVALUE_INT:
			file.WriteInt32(pData->pConstants[i].intValue);
			break;
		case SCRIPTVALUE_STRING:
			file.WriteString(*pData->pConstants[i].pStrValue);
			file.WriteByte(0);
			break;
		}
	}
	
	//Call table
	repeat(pData->nCallTableEntries, i)
	{
		file.WriteString(pData->pCallTable[i]);
		file.WriteByte(0);
	}

	return true;
}