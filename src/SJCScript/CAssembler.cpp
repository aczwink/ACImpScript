//Class Header
#include "CAssembler.h"
//Global
#include <iostream>
//SJCLib
#include <SJCLib.h>
//Local
#include "CRMAssembler.h"
//Namespaces
using namespace SJCLib;
//Definitions
#define REPORT_ERROR(msg) {stdErr << "Error on line " << this->lexer.GetCurrentLineNumber() << ": " << msg << endl; return false;}

//Constructor
CAssembler::CAssembler()
{
	this->nextCallIndex = 0;
}

//Private Functions
bool CAssembler::AssembleCodeBlock()
{
	Token t;

	ASSERT(lexer.GetNextToken() == TOKEN_CODE);

	while(t = lexer.GetNextToken())
	{
		switch(t)
		{
		case TOKEN_END:
			return true;
		case TOKEN_BLOCK:
			if(!this->AssembleDataBlock())
				return false;
			return true;
		case TOKEN_PROC:
			if(!this->AssembleProc())
			{
				return false;
			}
			break;
		default:
			ASSERT(false);
		}
	}

	return false;
}

bool CAssembler::AssembleDataBlock()
{
	Token t;

	ASSERT(lexer.GetNextToken() == TOKEN_DATA);

	while(t = lexer.GetNextToken())
	{
		switch(t)
		{
		case TOKEN_END:
			return this->CheckProgram();
		case TOKEN_LABEL:
			{
				SScriptConstant constant;
				CString name, strValue;
				
				name = lexer.GetCurrentTokenValue();
				name = name.SubString(0, name.GetLength()-1);
				t = lexer.GetNextToken();

				switch(t)
				{
				case TOKEN_LITERAL:
					strValue = lexer.GetCurrentTokenValue();
					strValue = strValue.SubString(1, strValue.GetLength() - 2);
					strValue.Replace("\\n", '\n');
					constant.pStrValue = new CString;
					constant.type = SCRIPTVALUE_STRING;
					*constant.pStrValue = strValue;
					this->constants[name] = constant;
					break;
				case TOKEN_NUMBER:
					constant.pStrValue = NULL;
					constant.intValue = lexer.GetCurrentTokenValue().ToInt32();
					constant.type = SCRIPTVALUE_INT;
					this->constants[name] = constant;
					break;
				case TOKEN_FLOAT:
					constant.fValue = lexer.GetCurrentTokenValue().ToFloat();
					constant.intValue = 0;
					constant.pStrValue = NULL;
					constant.type = SCRIPTVALUE_FLOAT;
					this->constants[name] = constant;
					break;
				default:
					ASSERT(false);
				}
			}
			break;
		default:
			ASSERT(false);
		}
	}

	return false;
}

bool CAssembler::AssembleProc()
{
	Token t;
	SScriptFunction function;

	ASSERT(lexer.GetNextToken() == TOKEN_IDENTIFIER);
	if(this->functions.Find(lexer.GetCurrentTokenValue()).IsValid())
	{
		ASSERT(false);
	}
	function.pName = new CString;
	*function.pName = lexer.GetCurrentTokenValue();
	function.offset = this->program.GetLength();

	while(t = lexer.GetNextToken())
	{
		switch(t)
		{
		case TOKEN_IDENTIFIER:
			stdErr << "Unknown opcode '" << this->lexer.GetCurrentTokenValue() << '\'' << " in line " << endl;
			return false;
			break;
		case TOKEN_OPCODE:
			if(!this->AssembleOpcode(lexer.GetCurrentTokenValue()))
			{
				delete function.pName;
				REPORT_ERROR("Failed to assemble opcode.");
			}
		case TOKEN_LABEL:
			{
				CString label;
				uint32 offset;
				CMapIterator<uint32, CString> it = this->offsetsToInsertLabels.Begin();

				label = this->lexer.GetCurrentTokenValue();
				label = label.SubString(0, label.GetLength()-1);
				offset = this->program.GetLength();

				this->labels[label] = offset;
			}
			break;
		case TOKEN_ENDKEYWORD:
			function.nOpcodes = this->program.GetLength() - function.offset;

			this->functions[*function.pName] = function;
			return this->CheckProc(*function.pName);
		default:
			ASSERT(false);
		}
	}

	return false;
}

bool CAssembler::CheckProc(const CString &procName)
{
	//Check if there are undefined labels
	for(CMap<uint32, CString>::Iterator it = this->offsetsToInsertLabels.Begin(); it.IsValid(); it++)
	{
		if(!this->labels.Find(it.GetValue()).IsValid())
		{
			stdErr << "Errors in procedure \"" << procName << "\":" << endl
				<< " -Label \"" << procName << "\" is undefined." << endl;
			return false;
		}
	}
	
	//Replace jump offsets
	for(CMap<CString, uint32>::Iterator it = this->labels.Begin(); it.IsValid(); it++)
	{
		for(CMap<uint32, CString>::Iterator it2 = this->offsetsToInsertLabels.Begin(); it2.IsValid(); it2++)
		{
			if(it2.GetValue() == it.GetKey())
			{
				this->program[it2.GetKey()] = it.GetValue() - it2.GetKey() - 1;
			}
		}
	}

	//Clear labels
	this->labels.Release();
	this->offsetsToInsertLabels.Release();
	
	return true;
}

bool CAssembler::CheckProgram()
{
	uint32 i;

	//Check if there are undefined constants
	for(CMap<uint32, CString>::Iterator it = this->offsetsToInsert.Begin(); it.IsValid(); it++)
	{
		if(!this->constants.Find(it.GetValue()).IsValid())
		{
			stdErr << "Error: Undefined constant \"" << it.GetValue() << "\"." << endl;
			return false;
		}
	}

	//Replace constants
	i = 0;
	for(CMap<CString, SScriptConstant>::Iterator it = this->constants.Begin(); it.IsValid(); it++)
	{
		SScriptConstant c = it.GetValue();
		
		for(CMap<uint32, CString>::Iterator it2 = this->offsetsToInsert.Begin(); it2.IsValid(); it2++)
		{
			if(it2.GetValue() == it.GetKey())
				this->program[it2.GetKey()] = i;
		}
		i++;
	}
	
	return true;
}

void CAssembler::Output(CString &filename)
{
	SScriptData data;
	uint32 i = 0;
	CMapIterator<CString, SScriptFunction> it;
	CMapIterator<CString, SScriptConstant> it2;
	CMapIterator<CString, uint32> it3;

	data.machineId = this->machineId;

	//Functions
	data.nFunctions = this->functions.GetNoOfElements();
	data.pFunctions = (SScriptFunction *)malloc(data.nFunctions * sizeof(*data.pFunctions));
	i = 0;
	it = this->functions.GetIterator();
	while(it.IsValid())
	{
		data.pFunctions[i++] = it.GetValue();
		it++;
	}

	//Program
	data.programSize = this->program.GetLength();
	data.pProgram = (Opcode *)malloc(data.programSize * sizeof(*data.pProgram));
	repeat(data.programSize, i)
	{
		data.pProgram[i] = this->program[i];
	}

	//Constants
	data.nConstants = this->constants.GetNoOfElements();
	data.pConstants = (SScriptConstant *)malloc(data.nConstants * sizeof(*data.pConstants));
	it2 = this->constants.GetIterator();
	i = 0;
	while(it2.IsValid())
	{
		data.pConstants[i++] = it2.GetValue();
		it2++;
	}

	//Call Table
	data.nCallTableEntries = this->callIndices.GetNoOfElements();
	it3 = this->callIndices.GetIterator();
	data.pCallTable = new CString[data.nCallTableEntries];
	i = 0;
	while(it3.IsValid())
	{
		data.pCallTable[it3.GetValue()] = it3.GetKey();
		it3++;
	}

	WriteCompiledScript(filename, &data);
	ReleaseCompiledScript(&data);
}

//Protected Functions
void CAssembler::AssembleJumpLabel()
{
	CString buffer;
	CMapIterator<CString, uint32> it = this->labels.GetIterator();
	bool found = false;
	
	ASSERT(this->lexer.GetNextToken() == TOKEN_IDENTIFIER);
	buffer = this->lexer.GetCurrentTokenValue();
	
	while(it.IsValid())
	{
		if(it.GetKey() == buffer)
		{
			this->program.Push(it.GetValue() - this->program.GetLength() - 1);
			found = true;
			break;
		}
		it++;
	}
	if(!found)
	{
		this->offsetsToInsertLabels[this->program.GetLength()] = buffer;
		this->program.Push(0);
	}
}

void CAssembler::AssembleSysCall()
{
	ESysCallId id;
	ASSERT(this->lexer.GetNextToken() == TOKEN_IDENTIFIER);

	switch(id = GetSysCallId(this->lexer.GetCurrentTokenValue()))
	{
	default:
	case SYSCALL_ILLEGAL:
		ASSERT(false);
		break;
	case SYSCALL_PRINTI:
	case SYSCALL_PRINTF:
	case SYSCALL_PRINTS:
	case SYSCALL_RAND:
		this->program.Push(id);
		break;
	}
}

//Public Functions
bool CAssembler::Assemble(CString outputFilename)
{
	Token t;
	
	while((t = this->lexer.GetNextToken()) != TOKEN_END)
	{
		switch(t)
		{
		case TOKEN_BLOCK:
			if(this->AssembleCodeBlock())
			{
				this->Output(outputFilename);
				
				return true;
			}
			return false;
		default:
			REPORT_ERROR("Expected \"" << KEYWORD_BLOCK << "\" keyword.");
		}
	}

	return false;
}

void CAssembler::Init(const char *pBuffer)
{
	this->lexer.Init(pBuffer);
}