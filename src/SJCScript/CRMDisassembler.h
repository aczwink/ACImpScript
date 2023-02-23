//Local
#include "CDisassembler.h"

class CRMDisassembler : public CDisassembler
{
private:
	//Functions
	void GetAllLabels();
	void Output2Registers(uint32 offset);
	void Output3Registers(uint32 offset);
	uint32 OutputOpcode(uint32 offset, COFStream &file);
	void OutputRegister(uint32 offset);
};