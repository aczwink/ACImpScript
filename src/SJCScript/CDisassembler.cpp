//Class Header
#include "CDisassembler.h"
//Global
#include <iostream>
//Local
#include "CAsmLexer.h"
#include "CompiledScript.h"

//Constructor
CDisassembler::CDisassembler()
{
	MemZero(&data, sizeof(data));
	this->nextLabelIndex = 0;
}

//Destructor
CDisassembler::~CDisassembler()
{
	ReleaseCompiledScript(&this->data);
}

//Private Functions
void CDisassembler::OutputProcedure(SScriptFunction *pFn, COFStream &file)
{
	uint32 i = 0;
	CMapIterator<uint32, uint32> it;

	file << KEYWORD_PROC << ' ' << *pFn->pName << endl;
	while(i < pFn->nOpcodes)
	{
		it = this->labels.Find(pFn->offset + i);
		if(it.IsValid())
		{
			file << "label" << CString(it.GetValue()) << ':' << endl;
		}

		file << '\t';
		i += this->OutputOpcode(pFn->offset + i, file);
		file << endl;
	}

	//Check if there is a label at the end
	it = this->labels.Find(pFn->offset + pFn->nOpcodes);
	if(it.IsValid())
	{
		file << "label" << CString(it.GetValue()) << ':' << endl;
	}

	file << KEYWORD_END << endl << endl;
}

//Public Functions
bool CDisassembler::Init(EVirtualMachine machineId, CString filename)
{
	if(!ReadCompiledScript(filename, &this->data))
	{
		stdErr << "Error: File \"" << filename << "\" is not a compiled script." << endl;
		return false;
	}
	if(this->data.machineId != machineId)
	{
		stdErr << "Error: This script is not compatible with this machine." << endl;
		return false;
	}
	
	return true;
}

bool CDisassembler::Output(CString filename)
{
	if(!file.Open(filename))
		return false;

	//Get All Jump Labels
	this->GetAllLabels();

	//Write Code block
	file << KEYWORD_BLOCK << ' ' << KEYWORD_CODE << endl << endl;
	repeat(this->data.nFunctions, i)
	{
		this->OutputProcedure(&this->data.pFunctions[i], file);
	}

	//Write Data Block
	file << KEYWORD_BLOCK << ' ' << KEYWORD_DATA << endl;
	repeat(this->data.nConstants, i)
	{
		file << "\tconst" << CString(i) << ": ";
		if(this->data.pConstants[i].type == SCRIPTVALUE_INT)
		{
			file << CString(this->data.pConstants[i].intValue);
		}
		else if(this->data.pConstants[i].type == SCRIPTVALUE_FLOAT)
		{
			file << CString(this->data.pConstants[i].fValue);
		}
		else
		{
			CString str = *this->data.pConstants[i].pStrValue;
			str.Replace('\n', "\\n");

			file << '"' << str << '"';
		}
		file << endl;
	}

	return true;
}