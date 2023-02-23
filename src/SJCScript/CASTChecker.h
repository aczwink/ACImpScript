//Local
#include "CompiledScript.h"
#include "CParser.h"

class CASTChecker
{
private:
	//Variables
	SFuncsAndConsts *pAST;
	CMap<CString, SConstant *> constants;
	CFiniteSet<int32> constNumbers;
	CFiniteSet<CString> constLiterals;
	CArray<SScriptConstant> resultingConstants;
	CMap<CString, ETypeType> localVariables;
	CMap<CString, SFunction *> functions;
	//Functions
	bool CheckArg(SArgument *pArgument);
	bool CheckConstant(SConstant *pConstant);
	bool CheckDeclaration(ETypeType type, SVarNames *pVarName);
	bool CheckExpression(SExpression *pExpression);
	bool CheckFunction(SFunction *pFunction);
	bool CheckStatement(SStatement *pStatement);
	bool CheckStatements(SStatements *pStatements);
	bool CheckValue(SValue *pValue);
	ETypeType GetType(SExpression *pExpression);
	ETypeType GetType(CString *pIdentifier);
	ESysCallId IsSysCall(CString *pIdentifier, SCallArg *pCallArg);
public:
	//Destructor
	~CASTChecker();
	//Functions
	bool Check();
	const CArray<SScriptConstant> &GetConstants() const;
	void Init(SFuncsAndConsts *pAST);
};