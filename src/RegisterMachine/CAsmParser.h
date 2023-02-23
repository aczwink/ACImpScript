//Libs
#include <SJCLib.h>
//Local
#include "AsmTokens.h"
#include "CAsmLexer.h"
#include "Opcodes.h"
//Namespaces
using namespace SJCLib;

/*
START -> BLOCKS;

BLOCKS -> BLOCKS2;
BLOCKS -> DATABLOCK BLOCKS2;
BLOCKS2 -> CODEBLOCK;

BLOCK -> DATABLOCK;
BLOCK -> CODEBLOCK;



DATABLOCK -> .data DATAENTRIES;



*/

enum EType
{
	TYPE_FLOAT32,
	TYPE_FLOAT64
};

enum EInstructionFormat
{
	IF_OPONLY,
	IF_IMMEDIATE,
	IF_REG,
	IF_REG_IMMEDIATE,
	IF_REG_REG_IMMEDIATE,
	IF_REG_REG_REG,
};

struct SInstruction
{
	EInstructionFormat format;
	EOpcode opcode;
	uint8 regDest;
	uint8 regSource1;
	uint8 regSource2;
	uint32 immediate;
	CString label;
	bool labelOnly;
};

struct SInstructionNameToOp
{
	CString name;
	EOpcode op;
};

//Recursive descent parser
class CAsmParser
{
private:
	//Members
	byte *pScript;
	CAsmLexer lexer;
	EToken lookahead;
	CFIFOBuffer dataSection;
	CMap<CString, uint32> dataLabels;
	EType currentType;
	CLinkedList<SInstruction> instructions;
	//Methods
	uint8 ExpectRegister();
	uint32 GetSysCallId();
	bool InInstructionSet(const SInstructionNameToOp *pMap, uint8 nOps, EOpcode &refOpcode);
	bool IsIdentifierInstruction(EOpcode &refOpcode);
	bool IsNoOperandInstruction(EOpcode &refOpcode);
	bool IsRegInstruction(EOpcode &refOpcode);
	bool IsRegAddressInstruction(EOpcode &refOpcode);
	bool IsRegIdentifierInstruction(EOpcode &refOpcode);
	bool IsRegRegInstruction(EOpcode &refOpcode);
	bool IsRegRegIdentifierInstruction(EOpcode &refOpcode);
	bool IsRegRegRegInstruction(EOpcode &refOpcode);
	void ParseAddress(SInstruction &refInstruction);
	void ParseCodeBlock();
	void ParseCodeEntries();
	void ParseDataBlock();
	void ParseData();
	void ParseDataEntries();
	void ParseDataEntry();
	bool ParseInstruction();
	//Inline
	inline bool Accept(EToken token)
	{
		if(this->lookahead == token)
		{
			this->NextLookahead();
			return true;
		}
		return false;
	}

	inline void Expect(EToken token)
	{
		ASSERT(this->lookahead == token);

		this->NextLookahead();
	}
	
	inline void NextLookahead()
	{
		this->lookahead = this->lexer.GetNextToken();
	}
public:
	//Constructor
	CAsmParser(const CString &refInputFileName);
	//Destructor
	~CAsmParser();
	//Methods
	void ParseBlocks();

	//Inline
	inline const CMap<CString, uint32> &GetDataLabels() const
	{
		return this->dataLabels;
	}

	inline CFIFOBuffer &GetDataSection()
	{
		return this->dataSection;
	}

	inline const CLinkedList<SInstruction> &GetInstructions() const
	{
		return this->instructions;
	}
};