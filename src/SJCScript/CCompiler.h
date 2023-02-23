//SJCLib
#include <SJCLib.h>
//Local
#include "CParser.h"
#include "CompiledScript.h"
//Namespaces
using namespace SJCLib;

class CCompiler
{
private:
	//Variables
	SFuncsAndConsts *pAST;
	CArray<SScriptFunction> functions;
	CArray<Opcode> program;
	const CArray<SScriptConstant> *pConstants;
	CMap<CString, uint32> localVariables;
	CMap<CString, uint32> callIndices;
	uint32 bspIndex;
	uint32 nextCallIndex;
	//Functions
	bool CompileDeclaration(ETypeType varType, SVarNames *pVarNames);
	bool CompileExpression(SExpression *pExpression);
	bool CompileFunction(SFunction *pFunction);
	bool CompileJump(SExpression *pExpression, uint32 &offset);
	bool CompileStatement(SStatement *pStatement);
	bool CompileStatements(SStatements *pStatements);
	uint32 FindConstantIndex(SValue *pValue);
public:
	//Constructor
	CCompiler();
	//Functions
	bool Compile();
	void Init(SFuncsAndConsts *pAST, const CArray<SScriptConstant> &constants);
	bool Output(CString filename);
};