#pragma once
//SJCLib
#include <SJCLib.h>
//Local
#include "Machines.h"
#include "Opcodes.h"
//Namespaces
using namespace SJCLib;

/*File Layout:
struct Header:
	char signature[5]; //SJCCS
	EVirtualMachine machineId;
	uint32 nFunctions;
	uint32 programSize; //not size in bytes but number of opcodes
	uint32 nConstants;
	uint32 nCallTableEntries;
struct data:
	struct[Header.nFunctions]:
		char name[...]; //zero terminated
		uint32 offset; //where procedure begins
		uint32 nOpcodes; //number of opcodes
	Opcode program[Header.programSize];
	struct[Header.nConstants]:
		EStackValueType type;
		float value; //If type == STACKVALUETYPE_FLOAT
		int32 value; //If type == STACKVALUETYPE_INT
		char value[...] //zeroterminated, if type == STACKVALUETYPE_STRING
	char[nCallTableEntries][...]; //zero terminated
*/


//Definitions
#define COMPILEDSCRIPT_SIGNATURE "SJCCS"
#define COMPILEDSCRIPT_SIGNATURELENGTH 5
#define COMPILEDSCRIPT_ENTRYPOINT "main"
#define COMPILEDSCRIPT_FILEEXT "sjccs"

//Enums
enum EScriptValue
{
	SCRIPTVALUE_ILLEGAL,
	SCRIPTVALUE_FLOAT,
	SCRIPTVALUE_INT,
	SCRIPTVALUE_STRING
};

//Structs
struct SScriptFunction
{
	CString *pName;
	uint32 offset;
	uint32 nOpcodes;
};

struct SScriptConstant
{
	EScriptValue type;
	int32 intValue;
	float fValue;
	CString *pStrValue;
};

struct SScriptData
{
	EVirtualMachine machineId;
	uint32 nFunctions;
	uint32 programSize;
	uint32 nConstants;
	uint32 nCallTableEntries;
	SScriptFunction *pFunctions;
	Opcode *pProgram;
	SScriptConstant *pConstants;
	CString *pCallTable;
};

//Unions
union UDataWord
{
	int32 intValue;
	float fValue;
	CString *pStrValue;
	UDataWord *pArray;
};

//Functions
bool ReadCompiledScript(CString filename, SScriptData *pData);
void ReleaseCompiledScript(SScriptData *pData);
bool WriteCompiledScript(CString filename, SScriptData *pData);