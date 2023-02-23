//Class Header
#include "CASTChecker.h"
//Local
#include "UncompiledScript.h"

//Destructor
CASTChecker::~CASTChecker()
{
}

//Private Functions
bool CASTChecker::CheckArg(SArgument *pArg)
{
	if(pArg)
	{
		this->localVariables.Insert(*pArg->pIdentifier, pArg->type);
		this->CheckArg(pArg->pNext);
	}

	return true;
}

bool CASTChecker::CheckConstant(SConstant *pConstant)
{
	this->constants[*pConstant->pName] = pConstant;
	
	return true;
}

bool CASTChecker::CheckDeclaration(ETypeType type, SVarNames *pVarName)
{
	this->localVariables.Insert(*pVarName->pVarName, type);
	return this->CheckValue(pVarName->pValue);
}

bool CASTChecker::CheckExpression(SExpression *pExpression)
{
	if(!pExpression)
		return true;

	switch(pExpression->type)
	{
	case EXPRESSIONTYPE_EQUALS:
	case EXPRESSIONTYPE_LESSTHAN:
	case EXPRESSIONTYPE_LESSOREQUAL:
	case EXPRESSIONTYPE_MINUS:
	case EXPRESSIONTYPE_MULTIPLY:
	case EXPRESSIONTYPE_PLUS:
		if(!this->CheckExpression(pExpression->pExprLeft))
			return false;
		if(!this->CheckExpression(pExpression->pExprRight))
			return false;
		break;
	case EXPRESSIONTYPE_CALL:
		{
			SCallArg *pCallArg;
			ESysCallId syscId;

			syscId = SYSCALL_ILLEGAL;
			syscId = this->IsSysCall(pExpression->pCallFunction, pExpression->pCallArgs);
			if(syscId != SYSCALL_ILLEGAL)
			{
				pExpression->type = EXPRESSIONTYPE_SYSCALL;
				pExpression->sysCallId = syscId;
			}

			pCallArg = pExpression->pCallArgs;
			while(pCallArg)
			{
				if(!this->CheckExpression(pCallArg->pExpression))
					return false;
				pCallArg = pCallArg->pNext;
			}
		}
		break;
	case EXPRESSIONTYPE_IDENTIFIER:
		{
			CMapIterator<CString, ETypeType> it;

			it = this->localVariables.Find(*pExpression->pIdentifier);
			if(!it.IsValid())
			{
				CMapIterator<CString, SConstant *> it2;

				it2 = this->constants.Find(*pExpression->pIdentifier);
				if(it2.IsValid())
				{
					this->CheckConstant(it2.GetValue());
					ReleaseExpression(pExpression, false);
					pExpression->type = EXPRESSIONTYPE_VALUE;
					pExpression->pValue = new SValue;
					pExpression->pValue->type = it2.GetValue()->pValue->type;
					switch(it2.GetValue()->pValue->type)
					{
					case VALUETYPE_NUMBER:
						pExpression->pValue->number = it2.GetValue()->pValue->number;
						break;
					case VALUETYPE_LITERAL:
						pExpression->pValue->pLiteral = new CString;
						*pExpression->pValue->pLiteral = *it2.GetValue()->pValue->pLiteral;
						break;
					}
					
					return this->CheckExpression(pExpression);
				}
			}
		}
		break;
	case EXPRESSIONTYPE_VALUE:
		return this->CheckValue(pExpression->pValue);
	}
	
	return true;
}

bool CASTChecker::CheckFunction(SFunction *pFunction)
{
	this->localVariables.Release();
	this->CheckArg(pFunction->pArgs);
	
	return this->CheckStatements(pFunction->pStatements);
}

bool CASTChecker::CheckStatement(SStatement *pStatement)
{
	if(!pStatement)
		return true;

	switch(pStatement->type)
	{
	case STATEMENTTYPE_DECLARATION:
		if(!this->CheckDeclaration(pStatement->declarationType, pStatement->pVarNames))
			return false;
		break;
	case STATEMENTTYPE_EXPRESSION:
	case STATEMENTTYPE_RETURNEXPRESSION:
		if(!this->CheckExpression(pStatement->pExpression))
			return false;
		break;
	case STATEMENTTYPE_FOR:
		if(!this->CheckStatement(pStatement->pForInit))
			return false;
		if(!this->CheckExpression(pStatement->pForCondition))
			return false;
		if(!this->CheckStatements(pStatement->pForBlock))
			return false;
		break;
	case STATEMENTTYPE_IF:
		if(!this->CheckExpression(pStatement->pIfCondition))
			return false;
		if(!this->CheckStatements(pStatement->pIfBlock))
			return false;
		break;
	}

	return true;
}

bool CASTChecker::CheckStatements(SStatements *pStatements)
{
	if(!pStatements)
		return true;

	if(!this->CheckStatement(pStatements->pStatement))
		return false;
	return this->CheckStatements(pStatements->pNext);
}

bool CASTChecker::CheckValue(SValue *pValue)
{
	if(!pValue)
	{
		this->constNumbers.Insert(0);
		return true;
	}
	
	switch(pValue->type)
	{
	case VALUETYPE_NUMBER:
		this->constNumbers.Insert(pValue->number);
		break;
	case VALUETYPE_LITERAL:
		pValue->pLiteral->Replace("\\n", '\n');
		this->constLiterals.Insert(*pValue->pLiteral);
	}

	return true;
}

ETypeType CASTChecker::GetType(SExpression *pExpression)
{
	switch(pExpression->type)
	{
	case EXPRESSIONTYPE_CALL:
		if(this->functions[*pExpression->pCallFunction]->pReturnType->rtype == RETURNTYPE_VOID)
			return TYPE_NONE;
		return this->functions[*pExpression->pCallFunction]->pReturnType->type;
	case EXPRESSIONTYPE_EQUALS:
	case EXPRESSIONTYPE_LESSTHAN:
	case EXPRESSIONTYPE_LESSOREQUAL:
	case EXPRESSIONTYPE_MINUS:
	case EXPRESSIONTYPE_MULTIPLY:
	case EXPRESSIONTYPE_PLUS:
		{
			ETypeType t1, t2;

			t1 = this->GetType(pExpression->pExprLeft);
			t2 = this->GetType(pExpression->pExprRight);

			if(t1 == t2)
				return t1;
		}
		break;
	case EXPRESSIONTYPE_IDENTIFIER:
		return this->GetType(pExpression->pIdentifier);
	case EXPRESSIONTYPE_INCREMENT:
		if(this->GetType(pExpression->pIdentifier) == TYPE_INT)
			return TYPE_INT;
		break;
	case EXPRESSIONTYPE_VALUE:
		switch(pExpression->pValue->type)
		{
		case VALUETYPE_NUMBER:
			return TYPE_INT;
		case VALUETYPE_LITERAL:
			return TYPE_STRING;
		}
		break;
	}

	return TYPE_ILLEGAL;
}

//Either a local var or a constant
ETypeType CASTChecker::GetType(CString *pIdentifier)
{
	CMapIterator<CString, ETypeType> it;

	it = this->localVariables.Find(*pIdentifier);
	if(it.IsValid())
		return it.GetValue();

	return this->constants[*pIdentifier]->type;
}

ESysCallId CASTChecker::IsSysCall(CString *pIdentifier, SCallArg *pCallArg)
{
	ETypeType type;

	type = TYPE_ILLEGAL;
	if(*pIdentifier == SYSCALL_PRINT && !pCallArg->pNext)
		type = this->GetType(pCallArg->pExpression);
	
	switch(type)
	{
	case TYPE_INT:
		return SYSCALL_PRINTI;
	case TYPE_STRING:
		return SYSCALL_PRINTS;
	}
	
	return SYSCALL_ILLEGAL;
}

//Public Functions
bool CASTChecker::Check()
{
	SFuncsAndConsts *pFAC;
	SScriptConstant constant;

	pFAC = this->pAST;
	while(pFAC)
	{
		if(pFAC->isFunction)
			this->functions.Insert(*pFAC->pFunction->pName, pFAC->pFunction);
		pFAC = pFAC->pNext;
	}

	pFAC = this->pAST;
	while(pFAC)
	{
		if(pFAC->isFunction)
			this->CheckFunction(pFAC->pFunction);
		else
			this->CheckConstant(pFAC->pConstant);
		pFAC = pFAC->pNext;
	}
	
	this->resultingConstants.Release();
	constant.type = SCRIPTVALUE_INT;
	for(CFiniteSet<int32>::Iterator it = this->constNumbers.GetIterator(); it.IsValid(); it++)
	{
		constant.intValue = it.GetValue();
		this->resultingConstants.Push(constant);
	}
	constant.type = SCRIPTVALUE_STRING;
	for(CFiniteSet<CString>::Iterator it = this->constLiterals.GetIterator(); it.IsValid(); it++)
	{
		constant.pStrValue = new CString;
		*constant.pStrValue = it.GetValue();
		this->resultingConstants.Push(constant);
	}
	
	return true;
}

const CArray<SScriptConstant> &CASTChecker::GetConstants() const
{
	return this->resultingConstants;
}

void CASTChecker::Init(SFuncsAndConsts *pAST)
{
	this->pAST = pAST;
}