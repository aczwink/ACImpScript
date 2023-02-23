//Local
#include "CAssembler.h"

class CRMAssembler : public CAssembler
{
private:
	//Functions
	void Assemble2Registers();
	void Assemble3Registers();
	bool AssembleOpcode(CString &opcode);
	bool AssembleRegister();
public:
	//Constructor
	CRMAssembler();
};