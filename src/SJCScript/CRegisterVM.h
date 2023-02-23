//Local
#include "CompiledScript.h"
//Namespaces
using namespace SJCLib;

class CRegisterVM
{
private:
	//Variables
	UDataWord *pConstants;
	uint32 nFunctions;
	SScriptFunction *pFunctions;
	Opcode *pProgram;
	uint32 *pCallOffsets;
	SScriptData *pScriptData;
public:
	//Constructor
	CRegisterVM();
	//Destructor
	~CRegisterVM();
	//Functions
	bool LoadModule(CString input);
	void Run();
};