//Class Header
#include "CCompiler.h"
//Global
#include <iostream>
//SJCLib
#include <SJCLib.h>
//Local
#include "CASTChecker.h"
#include "CParser.h"
//Namespaces
using namespace SJCLib;
//Definitions
#define JUMPOFFSET_UNKNOWN 0xFFFFFFFF

//Constructor
CCompiler::CCompiler()
{
	this->nextCallIndex = 0;
}

//Private Functions
bool CCompiler::CompileDeclaration(ETypeType varType, SVarNames *pVarNames)
{
	this->program.Push(OPCODE_LDC);
	this->program.Push(this->FindConstantIndex(pVarNames->pValue));
	this->localVariables.Insert(*pVarNames->pVarName, this->bspIndex++);

	return true;
}

bool CCompiler::CompileExpression(SExpression *pExpression)
{
	if(!pExpression)
		return true;
	
	switch(pExpression->type)
	{
	case EXPRESSIONTYPE_CALL:
		{
			SCallArg *pArgs;
			CArray<SExpression *> stack;
			
			pArgs = pExpression->pCallArgs;
			while(pArgs)
			{
				stack.Push(pArgs->pExpression);
				pArgs = pArgs->pNext;
			}
			while(!stack.IsEmpty())
			{
				if(!this->CompileExpression(stack.Pop()))
					return false;
			}
			this->program.Push(OPCODE_CALL);
			if(!this->callIndices.Find(*pExpression->pCallFunction).IsValid())
				this->callIndices[*pExpression->pCallFunction] = this->nextCallIndex++;
			this->program.Push(this->callIndices[*pExpression->pCallFunction]);
		}
		break;
	case EXPRESSIONTYPE_EQUALS:
		if(!this->CompileExpression(pExpression->pExprLeft))
			return false;
		if(!this->CompileExpression(pExpression->pExprRight))
			return false;
		break;
	case EXPRESSIONTYPE_IDENTIFIER:
		this->program.Push(OPCODE_PUSH);
		this->program.Push(0); //BSP
		this->program.Push(-(int32)this->localVariables[*pExpression->pIdentifier]);
		break;
	case EXPRESSIONTYPE_INCREMENT:
		this->program.Push(OPCODE_INC);
		this->program.Push(0); //BSP
		this->program.Push(this->localVariables[*pExpression->pIdentifier]);
		break;
	case EXPRESSIONTYPE_MINUS:
		if(!this->CompileExpression(pExpression->pExprRight))
			return false;
		if(!this->CompileExpression(pExpression->pExprLeft))
			return false;
		this->program.Push(OPCODE_SUB);
		break;
	case EXPRESSIONTYPE_MULTIPLY:
		if(!this->CompileExpression(pExpression->pExprLeft))
			return false;
		if(!this->CompileExpression(pExpression->pExprRight))
			return false;
		this->program.Push(OPCODE_MUL);
		break;
	case EXPRESSIONTYPE_PLUS:
		if(!this->CompileExpression(pExpression->pExprLeft))
			return false;
		if(!this->CompileExpression(pExpression->pExprRight))
			return false;
		this->program.Push(OPCODE_ADD);
		break;
	case EXPRESSIONTYPE_SYSCALL:
		if(!this->CompileExpression(pExpression->pCallArgs->pExpression))
			return false;
		this->program.Push(OPCODE_SYSC);
		this->program.Push(pExpression->sysCallId);
		break;
	case EXPRESSIONTYPE_VALUE:
		this->program.Push(OPCODE_LDC);
		this->program.Push(this->FindConstantIndex(pExpression->pValue));
		break;
	}
	return true;
}

bool CCompiler::CompileFunction(SFunction *pFunction)
{
	SScriptFunction function;
	SArgument *pArg;
	
	function.offset = this->program.GetLength();
	function.pName = new CString;
	*function.pName = *pFunction->pName;
	
	this->bspIndex = 0;
	this->localVariables.Release();

	//Arguments
	pArg = pFunction->pArgs;
	while(pArg)
	{
		this->localVariables.Insert(*pArg->pIdentifier, this->bspIndex++);
		pArg = pArg->pNext;
	}
	
	if(!this->CompileStatements(pFunction->pStatements))
		return false;

	if(pFunction->pReturnType->rtype == RETURNTYPE_VOID)
	{
		if(this->localVariables.GetNoOfElements())
		{
			repeat(this->localVariables.GetNoOfElements(), i)
				this->program.Push(OPCODE_POP);
		}
		this->program.Push(OPCODE_RET);
	}
	
	function.nOpcodes = this->program.GetLength() - function.offset;
	this->functions.Push(function);

	return true;
}

bool CCompiler::CompileJump(SExpression *pExpression, uint32 &offset)
{
	switch(pExpression->type)
	{
	case EXPRESSIONTYPE_EQUALS:
		if(!this->CompileExpression(pExpression->pExprLeft))
			return false;
		if(!this->CompileExpression(pExpression->pExprRight))
			return false;
		this->program.Push(OPCODE_JNE);
		if(offset == JUMPOFFSET_UNKNOWN)
		{
			offset = this->program.GetLength();
			this->program.Push(0);
		}
		else
		{
			this->program.Push((int32)offset - this->program.GetLength() - 1);
		}
		break;
	case EXPRESSIONTYPE_LESSTHAN:
		if(!this->CompileExpression(pExpression->pExprLeft))
			return false;
		if(!this->CompileExpression(pExpression->pExprRight))
			return false;
		this->program.Push(OPCODE_JLE);
		if(offset == JUMPOFFSET_UNKNOWN)
		{
			offset = this->program.GetLength();
			this->program.Push(0);
		}
		else
		{
			this->program.Push((int32)offset - this->program.GetLength() - 1);
		}
		break;
	case EXPRESSIONTYPE_LESSOREQUAL:
		if(!this->CompileExpression(pExpression->pExprLeft))
			return false;
		if(!this->CompileExpression(pExpression->pExprRight))
			return false;
		this->program.Push(OPCODE_JL);
		if(offset == JUMPOFFSET_UNKNOWN)
		{
			offset = this->program.GetLength();
			this->program.Push(0);
		}
		else
		{
			this->program.Push((int32)offset - this->program.GetLength() - 1);
		}
		break;
	}

	return true;
}

bool CCompiler::CompileStatement(SStatement *pStatement)
{
	if(pStatement == NULL)
		return true;

	switch(pStatement->type)
	{
	case STATEMENTTYPE_DECLARATION:
		if(!this->CompileDeclaration(pStatement->declarationType, pStatement->pVarNames))
			return false;
		break;
	case STATEMENTTYPE_EXPRESSION:
		if(!this->CompileExpression(pStatement->pExpression))
			return false;
		break;
	case STATEMENTTYPE_FOR:
		{
			CMap<CString, uint32> currentLocalVariables;
			uint32 conditionOffset, endLoopOffset;

			currentLocalVariables = this->localVariables;
			endLoopOffset = JUMPOFFSET_UNKNOWN;

			if(!this->CompileStatement(pStatement->pForInit))
				return false;
			conditionOffset = this->program.GetLength();
			if(!this->CompileJump(pStatement->pForCondition, endLoopOffset))
				return false;
			if(!this->CompileStatements(pStatement->pForBlock))
				return false;
			if(!this->CompileExpression(pStatement->pForUpdate))
				return false;
			this->program.Push(OPCODE_JMP);
			this->program.Push((int32)conditionOffset - this->program.GetLength() - 1);
			this->program[endLoopOffset] = this->program.GetLength() - (int32)endLoopOffset - 1;

			repeat(this->localVariables.GetNoOfElements() - currentLocalVariables.GetNoOfElements(), i)
			{
				this->program.Push(OPCODE_POP);
			}
			this->localVariables = currentLocalVariables;
		}
		break;
	case STATEMENTTYPE_IF:
		{
			CMap<CString, uint32> currentLocalVariables;
			uint32 endIfOffset;

			currentLocalVariables = this->localVariables;
			endIfOffset = JUMPOFFSET_UNKNOWN;

			if(!this->CompileJump(pStatement->pIfCondition, endIfOffset))
				return false;
			if(!this->CompileStatements(pStatement->pIfBlock))
				return false;
			this->program[endIfOffset] = this->program.GetLength() - (int32)endIfOffset - 1;

			repeat(this->localVariables.GetNoOfElements() - currentLocalVariables.GetNoOfElements(), i)
			{
				this->program.Push(OPCODE_POP);
			}
			this->localVariables = currentLocalVariables;
		}
		break;
	case STATEMENTTYPE_RETURNEXPRESSION:
		{
			if(!this->CompileExpression(pStatement->pExpression))
				return false;
			if(this->localVariables.GetNoOfElements())
			{
				this->program.Push(OPCODE_MOV);
				this->program.Push(0); //BSP
				this->program.Push(-(int32)this->localVariables.GetNoOfElements()+1); //bottom of local stack
				this->program.Push(1); //SP
				this->program.Push(0); //+0
				repeat(this->localVariables.GetNoOfElements(), i)
				{
					this->program.Push(OPCODE_POP);
				}
			}
			this->program.Push(OPCODE_RET);
		}
		break;
	}

	return true;
}

bool CCompiler::CompileStatements(SStatements *pStatements)
{
	if(!pStatements)
		return true;

	if(!this->CompileStatement(pStatements->pStatement))
		return false;
	return this->CompileStatements(pStatements->pNext);
}

uint32 CCompiler::FindConstantIndex(SValue *pValue)
{
	repeat(this->pConstants->GetLength(), i)
	{
		if(!pValue)
		{
			if((*this->pConstants)[i].type == SCRIPTVALUE_INT && (*this->pConstants)[i].intValue == 0)
				return i;
			continue;
		}

		if(pValue->type == VALUETYPE_NUMBER && (*this->pConstants)[i].type == SCRIPTVALUE_INT)
		{
			if(pValue->number == (*this->pConstants)[i].intValue)
				return i;
		}
		if(pValue->type == VALUETYPE_LITERAL && (*this->pConstants)[i].type == SCRIPTVALUE_STRING)
		{
			if(*pValue->pLiteral == *(*this->pConstants)[i].pStrValue)
				return i;
		}
	}

	//Impossible because checker would've noticed
	ASSERT(false);
	return -1; 
}

//Public Functions
bool CCompiler::Compile()
{
	SFuncsAndConsts *pFAC;

	if(this->pAST == NULL)
		return true;

	pFAC = this->pAST;
	while(pFAC)
	{
		if(pFAC->isFunction)
		{
			if(!this->CompileFunction(pFAC->pFunction))
				return false;
		}
		pFAC = pFAC->pNext;
	}

	return true;
}

void CCompiler::Init(SFuncsAndConsts *pAST, const CArray<SScriptConstant> &constants)
{
	this->pAST = pAST;
	this->pConstants = &constants;
}

bool CCompiler::Output(CString filename)
{
	SScriptData data;
	bool ok;

	data.machineId = VM_STACK;

	//Constants
	data.nConstants = this->pConstants->GetLength();
	data.pConstants = (SScriptConstant *)malloc(data.nConstants * sizeof(*data.pConstants));
	repeat(data.nConstants, i)
	{
		data.pConstants[i] = (*this->pConstants)[i];
	}

	//Functions
	data.nFunctions = this->functions.GetLength();
	data.pFunctions = (SScriptFunction *)malloc(data.nFunctions * sizeof(*data.pFunctions));
	repeat(data.nFunctions, i)
	{
		data.pFunctions[i] = this->functions[i];
	}

	//Program
	data.programSize = this->program.GetLength();
	data.pProgram = (Opcode *)malloc(data.programSize * sizeof(*data.pProgram));
	repeat(data.programSize, i)
	{
		data.pProgram[i] = this->program[i];
	}

	//Call table
	data.nCallTableEntries = this->callIndices.GetNoOfElements();
	data.pCallTable = new CString[data.nCallTableEntries];
	for(CMapIterator<CString, uint32> it = this->callIndices.Begin(); it.IsValid(); it++)
	{
		data.pCallTable[it.GetValue()] = it.GetKey();
	}

	ok = WriteCompiledScript(filename, &data);
	ReleaseCompiledScript(&data);

	return ok;
}