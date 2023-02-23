#pragma once
//SJCLib
#include <SJCLib.h>
//Local
#include "CSourceLexer.h"
#include "AbstractSyntaxTree.h"
//Namespaces
using namespace SJCLib;

enum EParserNonTerminals
{
	START,
	FUNCSANDCONSTS,
	FUNCTION,
	CONSTANT,
	RETURNTYPE,
	ARGUMENTS,
	STATEMENTS,
	STATEMENT,
	EXPRESSION,
	CALLARGS,
	VALUE,
	TYPE,
	MORETHANZEROARGUMENTS,
	VARNAMES,
	FORINIT,
	ARRAYVALUE,
	MORETHANZEROCALLARGS,
};

union USemanticStackValue
{
	SFuncsAndConsts *pFuncsAndConsts;
	SFunction *pFunction;
	SConstant *pConstant;
	SArgument *pArgs;
	CString *pStr;
	SExpression *pExpression;
	SStatement *pStatement;
	SStatements *pStatements;
	SValue *pValue;
	SCallArg *pCallArgs;
	SReturnType *pReturnType;
	ETypeType type;
	SVarNames *pVarNames;
};

//LALR LR(1) parser
class CParser
{
	typedef int32 State;
	typedef int32 Action; //|Action| is a state, - means reduction, + means shift
private:
	//Variables
	CSourceLexer lexer;
	SourceTokens::ESourceToken lookAhead;
	CArray<State> stateStack;
	CArray<USemanticStackValue> semanticStack;
	SFuncsAndConsts *pAST;
	CFiniteSet<CString *> strings;
	//Functions
	USemanticStackValue GetSemanticValue(uint32 pos);
	EParserNonTerminals Reduce(Action reductionRule);
public:
	//Constructor
	CParser();
	//Destructor
	~CParser();
	//Functions
	SFuncsAndConsts *GetAbstractSyntaxTree() const;
	bool Parse(const char *pSource);
	void Release();
};