//Local
#include "CAssembler.h"

class CSMAssembler : public CAssembler
{
private:
	//Functions
	void AssembleOffset();
	bool AssembleOpcode(CString &opcode);
public:
	//Constructor
	CSMAssembler();
};