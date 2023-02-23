//Local
#include "CDisassembler.h"

class CSMDisassembler : public CDisassembler
{
private:
	//Functions
	void GetAllLabels();
	void OutputOffset(uint32 offset, COFStream &file);
	uint32 OutputOpcode(uint32 offset, COFStream &file);
};