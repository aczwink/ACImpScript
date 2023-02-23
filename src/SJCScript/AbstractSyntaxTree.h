//SJCLib
#include <SJCLib.h>
//Local
#include "Opcodes.h"
//Namespaces
using namespace SJCLib;

//Forward declarations
struct SExpression;
struct SFunction;
struct SConstant;
struct SReturnType;
struct SStatement;
struct SStatements;
struct SValue;
struct SVarNames;

//Types
enum EExpressionType
{
	EXPRESSIONTYPE_CALL,
	EXPRESSIONTYPE_EQUALS,
	EXPRESSIONTYPE_IDENTIFIER,
	EXPRESSIONTYPE_INCREMENT,
	EXPRESSIONTYPE_LESSTHAN,
	EXPRESSIONTYPE_LESSOREQUAL,
	EXPRESSIONTYPE_MINUS,
	EXPRESSIONTYPE_MULTIPLY,
	EXPRESSIONTYPE_NEWARRAY,
	EXPRESSIONTYPE_PLUS,
	EXPRESSIONTYPE_SYSCALL,
	EXPRESSIONTYPE_VALUE,
};

enum EStatementType
{
	STATEMENTTYPE_ASSIGNMENT,
	STATEMENTTYPE_DECLARATION,
	STATEMENTTYPE_EXPRESSION,
	STATEMENTTYPE_FOR,
	STATEMENTTYPE_IF,
	STATEMENTTYPE_RETURNEXPRESSION,
};

enum EReturnType
{
	RETURNTYPE_VOID,
	RETURNTYPE_TYPE
};

enum EValueType
{
	VALUETYPE_NUMBER,
	VALUETYPE_LITERAL,
};

enum ETypeType
{
	TYPE_ILLEGAL,
	TYPE_NONE,
	TYPE_ARRAY,
	TYPE_INT,
	TYPE_STRING
};

struct SArgument
{
	ETypeType type;
	CString *pIdentifier;
	SArgument *pNext;
};

struct SCallArg
{
	SExpression *pExpression;
	SCallArg *pNext;
};

struct SExpression
{
	EExpressionType type;
	SValue *pValue;
	CString *pCallFunction;
	SCallArg *pCallArgs;
	ESysCallId sysCallId;
	CString *pIdentifier;
	SExpression *pExprLeft;
	SExpression *pExprRight;
	SExpression *pNewArraySizeExpr;
};

struct SFuncsAndConsts
{
	bool isFunction;
	SFunction *pFunction;
	SConstant *pConstant;
	SFuncsAndConsts *pNext;
};

struct SFunction
{
	SReturnType *pReturnType;
	CString *pName;
	SArgument *pArgs;
	SStatements *pStatements;
};

struct SConstant
{
	ETypeType type;
	CString *pName;
	SValue *pValue;
};

struct SReturnType
{
	EReturnType rtype;
	ETypeType type;
};

struct SStatement
{
	EStatementType type;
	SExpression *pExpression;
	ETypeType declarationType;
	SVarNames *pVarNames;
	SStatement *pForInit;
	SExpression *pForCondition;
	SExpression *pForUpdate;
	SStatements *pForBlock;
	SExpression *pIfCondition;
	SStatements *pIfBlock;
	CString *pIdentifier;
};

struct SStatements
{
	SStatement *pStatement;
	SStatements *pNext;
};

struct SValue
{
	EValueType type;
	int32 number;
	CString *pLiteral;
};

struct SVarNames
{
	CString *pVarName;
	SValue *pValue;
};

//Functions
ETypeType GetType(SExpression *pExpression);
ETypeType GetType(CString *pIdentifier);
void ReleaseExpression(SExpression *pExpression, bool deleteMe = true);
void ReleaseAST(SFuncsAndConsts *pAST);