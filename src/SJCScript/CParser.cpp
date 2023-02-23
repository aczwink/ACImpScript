//Class Header
#include "CParser.h"
//Namespaces
using namespace SourceTokens;
//Definitions
#define REDUCE(count) repeat(count, __idx){ stateStack.Pop(); this->semanticStack.Pop(); }
#define RESET lookAhead = TOKEN_ILLEGAL;
#define SHIFT(state) {USemanticStackValue s; s.pStr = new CString; *s.pStr = this->lexer.GetTokenValue(); stateStack.Push(state); this->semanticStack.Push(s); strings.Insert(s.pStr); }
#define TOPSTATE stateStack[stateStack.GetLength()-1]
#define SEMPUSH(value) this->semanticStack.Push(value)
#define SEMVAL(pos) this->GetSemanticValue(pos)

//Parser Tables
#define ACTION_ACCEPT 0xC0000000
#define ACTION_ERROR 0x80000000
#define GOTO_ERROR 0x80000000
#include "ParserTables.h"

//Constructor
CParser::CParser()
{
	this->pAST = NULL;
}

//Destructor
CParser::~CParser()
{
	this->Release();
}

//Private Functions
USemanticStackValue CParser::GetSemanticValue(uint32 pos)
{
	this->strings.Delete(this->semanticStack[this->semanticStack.GetLength()-1-pos].pStr);
	return this->semanticStack[this->semanticStack.GetLength()-1-pos];
}

EParserNonTerminals CParser::Reduce(Action reductionRule)
{
	USemanticStackValue gpv; //General-purpose value

	switch(reductionRule)
	{
	case 2:
		gpv.pFuncsAndConsts = NULL;
		SEMPUSH(gpv);
		return FUNCSANDCONSTS;
	case 3:
		{
			SFuncsAndConsts *pFAC;
			
			gpv.pFuncsAndConsts = SEMVAL(0).pFuncsAndConsts;
			if(gpv.pFuncsAndConsts)
			{
				pFAC = gpv.pFuncsAndConsts;
				while(pFAC->pNext) pFAC = pFAC->pNext;
				pFAC->pNext = new SFuncsAndConsts;
				pFAC = pFAC->pNext;
			}
			else
			{
				gpv.pFuncsAndConsts = new SFuncsAndConsts;
				pFAC = gpv.pFuncsAndConsts;
			}
			pFAC->isFunction = true;
			pFAC->pFunction = SEMVAL(1).pFunction;
			pFAC->pNext = NULL;
			REDUCE(2);
			SEMPUSH(gpv);
			return FUNCSANDCONSTS;
		}
	case 4:
		{
			gpv.pFuncsAndConsts = new SFuncsAndConsts;
			gpv.pFuncsAndConsts->isFunction = false;
			gpv.pFuncsAndConsts->pConstant = SEMVAL(1).pConstant;
			gpv.pFuncsAndConsts->pNext = SEMVAL(0).pFuncsAndConsts;
			REDUCE(2);
			SEMPUSH(gpv);
			return FUNCSANDCONSTS;
		}
	case 5:
		gpv.pFunction = new SFunction;
		gpv.pFunction->pReturnType = SEMVAL(7).pReturnType;
		gpv.pFunction->pName = SEMVAL(6).pStr;
		gpv.pFunction->pArgs = SEMVAL(4).pArgs;
		gpv.pFunction->pStatements = SEMVAL(1).pStatements;
		REDUCE(8);
		SEMPUSH(gpv);
		return FUNCTION;
	case 6:
		gpv.pConstant = new SConstant;
		gpv.pConstant->type = SEMVAL(4).type;
		gpv.pConstant->pName = SEMVAL(3).pStr;
		gpv.pConstant->pValue = SEMVAL(1).pValue;
		REDUCE(6);
		SEMPUSH(gpv);
		return CONSTANT;
	case 7:
		REDUCE(1);
		gpv.pReturnType = new SReturnType;
		gpv.pReturnType->rtype = RETURNTYPE_VOID;
		SEMPUSH(gpv);
		return RETURNTYPE;
	case 8:
		gpv.pReturnType = new SReturnType;
		gpv.pReturnType->rtype = RETURNTYPE_TYPE;
		gpv.pReturnType->type = SEMVAL(0).type;
		REDUCE(1);
		SEMPUSH(gpv);
		return RETURNTYPE;
	case 9:
		gpv.pArgs = NULL;
		SEMPUSH(gpv);
		return ARGUMENTS;
	case 10:
		SEMPUSH(SEMVAL(0));
		REDUCE(1);
		return ARGUMENTS;
	case 11:
		gpv.pArgs = new SArgument;
		gpv.pArgs->type = SEMVAL(1).type;
		gpv.pArgs->pIdentifier = SEMVAL(0).pStr;
		gpv.pArgs->pNext = NULL;
		REDUCE(2);
		SEMPUSH(gpv);
		return MORETHANZEROARGUMENTS;
	case 12:
		gpv.pArgs = new SArgument;
		gpv.pArgs->type = SEMVAL(3).type;
		gpv.pArgs->pIdentifier = SEMVAL(2).pStr;
		gpv.pArgs->pNext = SEMVAL(0).pArgs;
		REDUCE(4);
		SEMPUSH(gpv);
		return MORETHANZEROARGUMENTS;
	case 13:
		gpv.pStatements = NULL;
		SEMPUSH(gpv);
		return STATEMENTS;
	case 14:
		gpv.pStatements = new SStatements;
		gpv.pStatements->pStatement = SEMVAL(1).pStatement;
		gpv.pStatements->pNext = SEMVAL(0).pStatements;
		REDUCE(2);
		SEMPUSH(gpv);
		return STATEMENTS;
	case 15:
		gpv.pStatement = new SStatement;
		gpv.pStatement->type = STATEMENTTYPE_EXPRESSION;
		gpv.pStatement->pExpression = SEMVAL(1).pExpression;
		REDUCE(2);
		SEMPUSH(gpv);
		return STATEMENT;
	case 16:
		gpv.pStatement = new SStatement;
		gpv.pStatement->type = STATEMENTTYPE_DECLARATION;
		gpv.pStatement->pVarNames = SEMVAL(1).pVarNames;
		gpv.pStatement->declarationType = SEMVAL(2).type;
		REDUCE(3);
		SEMPUSH(gpv);
		return STATEMENT;
	case 18:
		gpv.pStatement = new SStatement;
		gpv.pStatement->type = STATEMENTTYPE_RETURNEXPRESSION;
		gpv.pStatement->pExpression = SEMVAL(1).pExpression;
		REDUCE(3);
		SEMPUSH(gpv);
		return STATEMENT;
	case 19:
		gpv.pStatement = new SStatement;
		gpv.pStatement->type = STATEMENTTYPE_IF;
		gpv.pStatement->pIfCondition = SEMVAL(4).pExpression;
		gpv.pStatement->pIfBlock = SEMVAL(1).pStatements;
		REDUCE(7);
		SEMPUSH(gpv);
		return STATEMENT;
	case 22:
		gpv.pStatement = new SStatement;
		gpv.pStatement->type = STATEMENTTYPE_FOR;
		gpv.pStatement->pForInit = SEMVAL(8).pStatement;
		gpv.pStatement->pForCondition = SEMVAL(6).pExpression;
		gpv.pStatement->pForUpdate = SEMVAL(4).pExpression;
		gpv.pStatement->pForBlock = SEMVAL(1).pStatements;
		REDUCE(11);
		SEMPUSH(gpv);
		return STATEMENT;
	case 23:
		gpv.pStatement = new SStatement;
		gpv.pStatement->type = STATEMENTTYPE_ASSIGNMENT;
		gpv.pStatement->pExpression = SEMVAL(1).pExpression;
		gpv.pStatement->pIdentifier = SEMVAL(3).pStr;
		REDUCE(4);
		SEMPUSH(gpv);
		return STATEMENT;
	case 24:
		gpv.type = TYPE_INT;
		REDUCE(1);
		SEMPUSH(gpv);
		return TYPE;
	case 27:
		gpv.type = TYPE_ARRAY;
		REDUCE(1);
		SEMPUSH(gpv);
		return TYPE;
	case 28:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_EQUALS;
		gpv.pExpression->pExprLeft = SEMVAL(2).pExpression;
		gpv.pExpression->pExprRight = SEMVAL(0).pExpression;
		REDUCE(3);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 30:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_LESSTHAN;
		gpv.pExpression->pExprLeft = SEMVAL(2).pExpression;
		gpv.pExpression->pExprRight = SEMVAL(0).pExpression;
		REDUCE(3);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 32:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_LESSOREQUAL;
		gpv.pExpression->pExprLeft = SEMVAL(2).pExpression;
		gpv.pExpression->pExprRight = SEMVAL(0).pExpression;
		REDUCE(3);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 34:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_PLUS;
		gpv.pExpression->pExprLeft = SEMVAL(2).pExpression;
		gpv.pExpression->pExprRight = SEMVAL(0).pExpression;
		REDUCE(3);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 35:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_MINUS;
		gpv.pExpression->pExprLeft = SEMVAL(2).pExpression;
		gpv.pExpression->pExprRight = SEMVAL(0).pExpression;
		REDUCE(3);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 36:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_MULTIPLY;
		gpv.pExpression->pExprLeft = SEMVAL(2).pExpression;
		gpv.pExpression->pExprRight = SEMVAL(0).pExpression;
		REDUCE(3);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 42:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_NEWARRAY;
		gpv.pExpression->pNewArraySizeExpr = SEMVAL(1).pExpression;
		REDUCE(5);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 43:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_INCREMENT;
		gpv.pExpression->pIdentifier = SEMVAL(1).pStr;
		REDUCE(2);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 45:
		gpv.pExpression = new SExpression;
		gpv.pExpression->pCallArgs = SEMVAL(1).pCallArgs;
		gpv.pExpression->pCallFunction = SEMVAL(3).pStr;
		gpv.pExpression->type = EXPRESSIONTYPE_CALL;
		REDUCE(4);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 46:
		gpv.pExpression = new SExpression;
		gpv.pExpression->type = EXPRESSIONTYPE_IDENTIFIER;
		gpv.pExpression->pIdentifier = SEMVAL(0).pStr;
		REDUCE(1);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 48:
		gpv.pExpression = new SExpression;
		gpv.pExpression->pValue = SEMVAL(0).pValue;
		gpv.pExpression->type = EXPRESSIONTYPE_VALUE;
		REDUCE(1);
		SEMPUSH(gpv);
		return EXPRESSION;
	case 49:
		gpv.pValue = new SValue;
		gpv.pValue->type = VALUETYPE_NUMBER;
		gpv.pValue->number = SEMVAL(0).pStr->ToInt32();
		delete SEMVAL(0).pStr;
		REDUCE(1);
		SEMPUSH(gpv);
		return VALUE;
	case 51:
		gpv.pValue = new SValue;
		gpv.pValue->type = VALUETYPE_LITERAL;
		gpv.pValue->pLiteral = SEMVAL(0).pStr;
		REDUCE(1);
		SEMPUSH(gpv);
		return VALUE;
	case 55:
		gpv.pVarNames = new SVarNames;
		gpv.pVarNames->pValue = NULL;
		gpv.pVarNames->pVarName = SEMVAL(0).pStr;
		REDUCE(1);
		SEMPUSH(gpv);
		return VARNAMES;
	case 56:
		gpv.pVarNames = new SVarNames;
		gpv.pVarNames->pValue = SEMVAL(0).pValue;
		gpv.pVarNames->pVarName = SEMVAL(2).pStr;
		REDUCE(3);
		SEMPUSH(gpv);
		return VARNAMES;
	case 60:
		SEMPUSH(SEMVAL(0));
		REDUCE(1);
		return CALLARGS;
	case 61:
		gpv.pCallArgs = new SCallArg;
		gpv.pCallArgs->pExpression = SEMVAL(0).pExpression;
		gpv.pCallArgs->pNext = NULL;
		REDUCE(1);
		SEMPUSH(gpv);
		return MORETHANZEROCALLARGS;
	case 62:
		gpv.pCallArgs = new SCallArg;
		gpv.pCallArgs->pExpression = SEMVAL(2).pExpression;
		gpv.pCallArgs->pNext = SEMVAL(0).pCallArgs;
		REDUCE(3);
		SEMPUSH(gpv);
		return MORETHANZEROCALLARGS;
	case 64:
		gpv.pStatement = new SStatement;
		gpv.pStatement->type = STATEMENTTYPE_DECLARATION;
		gpv.pStatement->declarationType = SEMVAL(1).type;
		gpv.pStatement->pVarNames = SEMVAL(0).pVarNames;
		REDUCE(2);
		SEMPUSH(gpv);
		return FORINIT;
	}
	ASSERT(false);
	return START; //ERROR
}

//Public Functions
SFuncsAndConsts *CParser::GetAbstractSyntaxTree() const
{
	return this->pAST;
}

bool CParser::Parse(const char *pSource)
{
	Action currentAction;
	
	this->lexer.Init(pSource);
	this->lookAhead = TOKEN_ILLEGAL;
	stateStack.Push(0);
	
	while(true)
	{
		if(this->lookAhead == TOKEN_ILLEGAL)
		{
			if(this->lookAhead == TOKEN_END)
			{
				return false;
			}
			this->lookAhead = this->lexer.GetNextToken();
			if(this->lookAhead == TOKEN_ILLEGAL)
			{
				return false;
			}
		}
		currentAction = actionTable[TOPSTATE * (TOKEN_FLOATNUMBER+1) + this->lookAhead];
		if(currentAction == ACTION_ERROR)
		{
			return false;
		}
		else if(currentAction == ACTION_ACCEPT)
		{
			this->pAST = this->semanticStack.Pop().pFuncsAndConsts;
			return true;
		}
		else if(currentAction > 0)
		{
			SHIFT(currentAction);
			RESET;
		}
		else
		{
			EParserNonTerminals nt;
			uint32 gotoState;

			nt = this->Reduce(-currentAction);
			gotoState = gotoTable[TOPSTATE * (MORETHANZEROCALLARGS+1) + nt];
			if(gotoState == GOTO_ERROR)
				return false;
			stateStack.Push(gotoState);
		}
	}

	return false;
}

void CParser::Release()
{
	for(CFiniteSet<CString *>::Iterator it = this->strings.Begin(); it.IsValid(); it++)
	{
		delete it.GetValue();
	}
	this->lookAhead = TOKEN_ILLEGAL;
	this->pAST = NULL;
	this->semanticStack.Release();
	this->stateStack.Release();
	this->strings.Release();
}