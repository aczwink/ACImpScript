//Class Header
#include "CAsmLexer.h"
//Local
#include "AsmLexerTable.h"

//Constructor
CAsmLexer::CAsmLexer() : CLexer(translateTable, transitionsTable, tokenTypeTable)
{
}

//Private methods
bool CAsmLexer::OnNextToken(const EToken &refParsedToken)
{
	switch(refParsedToken)
	{
	case TOKEN_LINEFEED:
	case TOKEN_WHITESPACE:
	case TOKEN_SINGLELINECOMMENT:
	case TOKEN_MULTILINECOMMENT:
		return false;
	case TOKEN_LABEL:
		this->tokenValue = this->tokenValue.SubString(0, this->tokenValue.GetLength()-1);
		return true;
	}

	return true;
}