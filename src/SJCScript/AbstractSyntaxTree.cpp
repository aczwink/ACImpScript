//Main Header
#include "AbstractSyntaxTree.h"

//Internal Function Prototypes
void ReleaseArgument(SArgument *pArgument);
void ReleaseStatement(SStatement *pStatement);
void ReleaseStatements(SStatements *pStatement);
void ReleaseValue(SValue *pValue);
void ReleaseVarName(SVarNames *pVarName);

//Internal Functions
void ReleaseArgument(SArgument *pArgument)
{
	if(pArgument)
	{
		delete pArgument->pIdentifier;
		ReleaseArgument(pArgument->pNext);
		delete pArgument;
	}
}

void ReleaseCallArg(SCallArg *pCallArg)
{
	if(pCallArg)
	{
		ReleaseExpression(pCallArg->pExpression);
		ReleaseCallArg(pCallArg->pNext);
		delete pCallArg;
	}
}

void ReleaseExpression(SExpression *pExpression, bool deleteMe)
{
	switch(pExpression->type)
	{
	case EXPRESSIONTYPE_CALL:
		ReleaseCallArg(pExpression->pCallArgs);
		delete pExpression->pCallFunction;
		break;
	case EXPRESSIONTYPE_EQUALS:
	case EXPRESSIONTYPE_LESSTHAN:
	case EXPRESSIONTYPE_LESSOREQUAL:
	case EXPRESSIONTYPE_MINUS:
	case EXPRESSIONTYPE_MULTIPLY:
	case EXPRESSIONTYPE_PLUS:
		ReleaseExpression(pExpression->pExprLeft);
		ReleaseExpression(pExpression->pExprRight);
		break;
	case EXPRESSIONTYPE_IDENTIFIER:
	case EXPRESSIONTYPE_INCREMENT:
		delete pExpression->pIdentifier;
		break;
	case EXPRESSIONTYPE_SYSCALL:
		ReleaseExpression(pExpression->pCallArgs->pExpression);
		HeapDelete(pExpression->pCallArgs);
		delete pExpression->pCallFunction;
		break;
	case EXPRESSIONTYPE_VALUE:
		ReleaseValue(pExpression->pValue);
		break;
	}
	if(deleteMe)
		delete pExpression;
}

void ReleaseFunction(SFunction *pFunction)
{
	delete pFunction->pName;
	delete pFunction->pReturnType;
	ReleaseArgument(pFunction->pArgs);
	ReleaseStatements(pFunction->pStatements);
	delete pFunction;
}

void ReleaseConstant(SConstant *pConstant)
{
	delete pConstant->pName;
	ReleaseValue(pConstant->pValue);
	delete pConstant;
}

void ReleaseStatement(SStatement *pStatement)
{
	if(!pStatement)
		return;

	switch(pStatement->type)
	{
	case STATEMENTTYPE_DECLARATION:
		ReleaseVarName(pStatement->pVarNames);
		break;
	case STATEMENTTYPE_EXPRESSION:
	case STATEMENTTYPE_RETURNEXPRESSION:
		ReleaseExpression(pStatement->pExpression);
		break;
	case STATEMENTTYPE_FOR:
		ReleaseStatement(pStatement->pForInit);
		ReleaseExpression(pStatement->pForCondition);
		ReleaseExpression(pStatement->pForUpdate);
		ReleaseStatements(pStatement->pForBlock);
		break;
	case STATEMENTTYPE_IF:
		ReleaseExpression(pStatement->pIfCondition);
		ReleaseStatements(pStatement->pIfBlock);
		break;
	}
	
	delete pStatement;
}

void ReleaseStatements(SStatements *pStatements)
{
	if(pStatements)
	{
		ReleaseStatement(pStatements->pStatement);
		ReleaseStatements(pStatements->pNext);
		delete pStatements;
	}
}

void ReleaseValue(SValue *pValue)
{
	if(!pValue)
		return;
	
	switch(pValue->type)
	{
	case VALUETYPE_LITERAL:
		delete pValue->pLiteral;
		break;
	}
	delete pValue;
}

void ReleaseVarName(SVarNames *pVarName)
{
	if(pVarName)
	{
		ReleaseValue(pVarName->pValue);
		delete pVarName->pVarName;
		delete pVarName;
	}
}

//Functions
void ReleaseAST(SFuncsAndConsts *pAST)
{
	SFuncsAndConsts *pFAC, *pFACDelete;
	
	pFAC = pAST;
	while(pFAC)
	{
		if(pFAC->isFunction)
			ReleaseFunction(pFAC->pFunction);
		else
			ReleaseConstant(pFAC->pConstant);
		
		pFACDelete = pFAC;
		pFAC = pFAC->pNext;
		delete pFACDelete;
	}
}