#pragma once
//SJCLib
#include <SJCLib.h>
//Local
#include "Opcodes.h"
#include "RegisterMachine.h"
//Namespaces
using namespace SJCLib;
//Definitions
#define KEYWORD_BLOCK "BLOCK"
#define KEYWORD_CODE "CODE"
#define KEYWORD_PROC "PROC"
#define KEYWORD_END "END"
#define KEYWORD_DATA "DATA"
#define KEYWORD_SP "SP"
#define KEYWORD_BSP "BSP"

enum CharClass
{
	CC_ILLEGAL,
	CC_NULLBYTE,
	CC_TABSPACE,
	CC_LINEFEED,
	CC_CARRIAGERETURN,
	CC_INVERTEDCOMMA,
	CC_NUMBERSIGN,
	CC_PLUS,
	CC_COMMA,
	CC_MINUS,
	CC_DOT,
	CC_NUMBER,
	CC_LETTER,
	CC_UNDERSCORE,
	CC_COLON,
	CC_INVALID
};

enum TokenState
{
	STATE_ILLEGAL,
	STATE_START,
	STATE_FINISHED,
	STATE_END,
	STATE_TABSPACE,
	STATE_CARRIAGERETURN,
	STATE_UNCLOSEDLITERAL,
	STATE_LITERAL,
	STATE_NUMBERSIGN,
	STATE_PLUS,
	STATE_LINEFEED,
	STATE_COMMA,
	STATE_MINUS,
	STATE_NUMBER,
	STATE_UNFINISHEDFLOAT,
	STATE_FLOAT,
	STATE_LETTER,
	STATE_LABEL
};

enum Token
{
	TOKEN_ILLEGAL,
	TOKEN_END,
	TOKEN_TABSPACE,
	TOKEN_CARRIAGERETURN,
	TOKEN_LINEFEED,
	TOKEN_LITERAL,
	TOKEN_COMMA,
	TOKEN_IDENTIFIER,
	TOKEN_COMMENT,
	TOKEN_BLOCK,
	TOKEN_CODE,
	TOKEN_PROC,
	TOKEN_ENDKEYWORD,
	TOKEN_DATA,
	TOKEN_SP,
	TOKEN_BSP,
	TOKEN_OPCODE,
	TOKEN_LABEL,
	TOKEN_NUMBER,
	TOKEN_FLOAT
};

#define CMP(str,ret) if(tokenValue == str) return ret;
static Token GetKeywordToken(CString &tokenValue)
{
	CMP(KEYWORD_BLOCK, TOKEN_BLOCK);
	CMP(KEYWORD_CODE, TOKEN_CODE);
	CMP(KEYWORD_PROC, TOKEN_PROC);
	CMP(KEYWORD_END, TOKEN_ENDKEYWORD);
	CMP(KEYWORD_DATA, TOKEN_DATA);
	CMP(KEYWORD_SP, TOKEN_SP);
	CMP(KEYWORD_BSP, TOKEN_BSP);

	if(GetOpcode(tokenValue) != OPCODE_ILLEGAL)
	{
		return TOKEN_OPCODE;
	}

	if(RM_GetOpcode(tokenValue) != OPCODE_ILLEGAL)
	{
		return TOKEN_OPCODE;
	}

	return TOKEN_IDENTIFIER;
}

class CAsmLexer
{
private:
	//Variables
	CString tokenValue;
	char *pSource;
	uint32 currentLineNumber;
	//Functions
	CharClass EvaluateChar(char c);
	Token GetTokenType(TokenState state);
	TokenState NextState(TokenState state, CharClass c);
public:
	//Constructor
	CAsmLexer();
	//Destructor
	~CAsmLexer();
	//Functions
	uint32 GetCurrentLineNumber() const;
	CString GetCurrentTokenValue();
	void Init(const char *pSource);
	Token GetNextToken();
};