//Class Header
#include "CSourceLexer.h"
//Namespaces
using namespace SourceTokens;

//Private Functions
CSourceLexer::ECharClass CSourceLexer::GetCharClass(char c)
{
	switch(c)
	{
	case 0:
		return CC_NULLBYTE;
	case 9: //TAB
	case 32: //Space
		return CC_TABSPACE;
	case 10: //\n
		return CC_LINEFEED;
	case 13: //\r
		return CC_CARRIAGERETURN;
	case 34: //"
		return CC_INVERTEDCOMMA;
	case 40: //(
		return CC_OPENPAREN;
	case 41: //)
		return CC_CLOSEPAREN;
	case 42: //*
		return CC_MULTIPLY;
	case 43: //+
		return CC_PLUS;
	case 44: //,
		return CC_COMMA;
	case 45: //-
		return CC_MINUS;
	case 47: ///
		return CC_SLASH;
	case 48: //0
	case 49: //1
	case 50: //2
	case 51: //3
	case 52: //4
	case 53: //5
	case 54: //6
	case 55: //7
	case 56: //8
	case 57: //9
		return CC_NUMBER;
	case 59: //;
		return CC_SEMICOLON;
	case 60: //<
		return CC_LESSTHAN;
	case 61: //=
		return CC_EQUALS;
	case 91: //[
		return CC_SQUAREDBRACKETOPEN;
	case 93: //]
		return CC_SQUAREDBRACKETCLOSE;
	case 65: //A
	case 66: //B
	case 67: //C
	case 68: //D
	case 69: //E
	case 70: //F
	case 71: //G
	case 72: //H
	case 73: //I
	case 74: //J
	case 75: //K
	case 76: //L
	case 77: //M
	case 78: //N
	case 79: //O
	case 80: //P
	case 81: //Q
	case 82: //R
	case 83: //S
	case 84: //T
	case 85: //U
	case 86: //V
	case 87: //W
	case 88: //X
	case 89: //Y
	case 90: //Z
	case 97: //a
	case 98: //b
	case 99: //c
	case 100: //d
	case 101: //e
	case 102: //f
	case 103: //g
	case 104: //h
	case 105: //i
	case 106: //j
	case 107: //k
	case 108: //l
	case 109: //m
	case 110: //n
	case 111: //o
	case 112: //p
	case 113: //q
	case 114: //r
	case 115: //s
	case 116: //t
	case 117: //u
	case 118: //v
	case 119: //w
	case 120: //x
	case 121: //y
	case 122: //z
		return CC_LETTER;
	case 123: //{
		return CC_OPENBRACE;
	case 125: //}
		return CC_CLOSEBRACE;
	case 33: //!
	case 58: //:
	case 92: //Backslash
	case 94: //^
		return CC_INVALID;
	}

	return CC_ILLEGAL;
}

CSourceLexer::EState CSourceLexer::GetNextState(EState state, ECharClass cc)
{
	switch(state)
	{
	case STATE_START:
		switch(cc)
		{
		case CC_NULLBYTE:
			return STATE_END;
		case CC_TABSPACE:
			return STATE_TABSPACE;
		case CC_LINEFEED:
			return STATE_LINEFEED;
		case CC_CARRIAGERETURN:
			return STATE_CARRIAGERETURN;
		case CC_INVERTEDCOMMA:
			return STATE_UNCLOSEDLITERAL;
		case CC_OPENPAREN:
			return STATE_OPENPAREN;
		case CC_CLOSEPAREN:
			return STATE_CLOSEPAREN;
		case CC_MULTIPLY:
			return STATE_MULTIPLY;
		case CC_PLUS:
			return STATE_PLUS;
		case CC_COMMA:
			return STATE_COMMA;
		case CC_MINUS:
			return STATE_MINUS;
		case CC_SLASH:
			return STATE_SLASH;
		case CC_NUMBER:
			return STATE_NUMBER;
		case CC_SEMICOLON:
			return STATE_SEMICOLON;
		case CC_LESSTHAN:
			return STATE_LESSTHAN;
		case CC_EQUALS:
			return STATE_ASSIGN;
		case CC_SQUAREDBRACKETOPEN:
			return STATE_SQUAREDBRACKETOPEN;
		case CC_SQUAREDBRACKETCLOSE:
			return STATE_SQUAREDBRACKETCLOSE;
		case CC_LETTER:
			return STATE_IDENTIFIER;
		case CC_OPENBRACE:
			return STATE_OPENBRACE;
		case CC_CLOSEBRACE:
			return STATE_CLOSEBRACE;
		}
		break;
	case STATE_END:
		return STATE_FINISHED;
		break;
	case STATE_TABSPACE:
		switch(cc)
		{
		case CC_TABSPACE:
			return STATE_TABSPACE;
		case CC_CARRIAGERETURN:
		case CC_MULTIPLY:
		case CC_PLUS:
		case CC_SLASH:
		case CC_NUMBER:
		case CC_LESSTHAN:
		case CC_EQUALS:
		case CC_LETTER:
		case CC_OPENBRACE:
		case CC_CLOSEBRACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_LINEFEED:
		switch(cc)
		{
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
		case CC_SLASH:
		case CC_LETTER:
		case CC_OPENBRACE:
		case CC_CLOSEBRACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_CARRIAGERETURN:
		switch(cc)
		{
		case CC_LINEFEED:
			return STATE_FINISHED;
		}
		break;
	case STATE_UNCLOSEDLITERAL:
		switch(cc)
		{
		case CC_TABSPACE:
		case CC_OPENPAREN:
		case CC_CLOSEPAREN:
		case CC_EQUALS:
		case CC_LETTER:
		case CC_INVALID:
			return STATE_UNCLOSEDLITERAL;
		case CC_INVERTEDCOMMA:
			return STATE_CLOSEDLITERAL;
		}
		break;
	case STATE_CLOSEDLITERAL:
		switch(cc)
		{
		case CC_CLOSEPAREN:
			return STATE_FINISHED;
		}
		break;
	case STATE_OPENPAREN:
		switch(cc)
		{
		case CC_INVERTEDCOMMA:
		case CC_CLOSEPAREN:
		case CC_NUMBER:
		case CC_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_CLOSEPAREN:
		switch(cc)
		{
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
		case CC_CLOSEPAREN:
		case CC_SEMICOLON:
			return STATE_FINISHED;
		}
		break;
	case STATE_MULTIPLY:
		switch(cc)
		{
		case CC_TABSPACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_PLUS:
		switch(cc)
		{
		case CC_PLUS:
			return STATE_PLUSPLUS;
		case CC_TABSPACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_COMMA:
		switch(cc)
		{
		case CC_TABSPACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_PLUSPLUS:
		switch(cc)
		{
		case CC_CLOSEPAREN:
			return STATE_FINISHED;
		}
		break;
	case STATE_MINUS:
		switch(cc)
		{
		case CC_NUMBER:
			return STATE_FINISHED;
		}
		break;
	case STATE_SLASH:
		switch(cc)
		{
		case CC_MULTIPLY:
			return STATE_MULTILINECOMMENT;
		case CC_SLASH:
			return STATE_SINGLELINECOMMENT;
		}
		break;
	case STATE_NUMBER:
		switch(cc)
		{
		case CC_NUMBER:
			return STATE_NUMBER;
		case CC_TABSPACE:
		case CC_CLOSEPAREN:
		case CC_SEMICOLON:
			return STATE_FINISHED;
		}
		break;
	case STATE_SINGLELINECOMMENT:
	case STATE_MULTILINECOMMENT:
		return STATE_FINISHED;
	case STATE_SEMICOLON:
		switch(cc)
		{
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
			return STATE_FINISHED;
		}
		break;
	case STATE_LESSTHAN:
		switch(cc)
		{
		case CC_TABSPACE:
			return STATE_FINISHED;
		case CC_EQUALS:
			return STATE_LESSOREQUAL;
		}
		break;
	case STATE_LESSOREQUAL:
		switch(cc)
		{
		case CC_TABSPACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_ASSIGN:
		switch(cc)
		{
		case CC_TABSPACE:
			return STATE_FINISHED;
		case CC_EQUALS:
			return STATE_EQUALS;
		}
		break;
	case STATE_SQUAREDBRACKETOPEN:
		switch(cc)
		{
		case CC_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_SQUAREDBRACKETCLOSE:
		switch(cc)
		{
		case CC_SEMICOLON:
			return STATE_FINISHED;
		}
		break;
	case STATE_EQUALS:
		switch(cc)
		{
		case CC_TABSPACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_IDENTIFIER:
		switch(cc)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_OPENPAREN:
		case CC_CLOSEPAREN:
		case CC_PLUS:
		case CC_COMMA:
		case CC_MINUS:
		case CC_SEMICOLON:
		case CC_SQUAREDBRACKETOPEN:
		case CC_SQUAREDBRACKETCLOSE:
			return STATE_FINISHED;
		case CC_LETTER:
			return STATE_IDENTIFIER;
		}
		break;
	case STATE_OPENBRACE:
		switch(cc)
		{
		case CC_CARRIAGERETURN:
			return STATE_FINISHED;
		}
		break;
	case STATE_CLOSEBRACE:
		switch(cc)
		{
		case CC_NULLBYTE:
		case CC_CARRIAGERETURN:
			return STATE_FINISHED;
		}
		break;
	}
	return STATE_ILLEGAL;
}

ESourceToken CSourceLexer::GetTokenType(EState state)
{
	switch(state)
	{
	case STATE_END:
		return TOKEN_END;
	case STATE_TABSPACE:
		return TOKEN_TABSPACE;
	case STATE_LINEFEED:
		return TOKEN_LINEFEED;
	case STATE_CARRIAGERETURN:
		return TOKEN_CARRIAGERETURN;
	case STATE_CLOSEDLITERAL:
		return TOKEN_LITERAL;
	case STATE_OPENPAREN:
		return TOKEN_OPENPAREN;
	case STATE_CLOSEPAREN:
		return TOKEN_CLOSEPAREN;
	case STATE_MULTIPLY:
		return TOKEN_MULTIPLY;
	case STATE_PLUS:
		return TOKEN_PLUS;
	case STATE_COMMA:
		return TOKEN_COMMA;
	case STATE_PLUSPLUS:
		return TOKEN_PLUSPLUS;
	case STATE_MINUS:
		return TOKEN_MINUS;
	case STATE_NUMBER:
		return TOKEN_NUMBER;
	case STATE_SINGLELINECOMMENT:
		return TOKEN_SINGLELINECOMMENT;
	case STATE_MULTILINECOMMENT:
		return TOKEN_MULTILINECOMMENT;
	case STATE_SEMICOLON:
		return TOKEN_SEMICOLON;
	case STATE_LESSTHAN:
		return TOKEN_LESSTHAN;
	case STATE_LESSOREQUAL:
		return TOKEN_LESSOREQUAL;
	case STATE_ASSIGN:
		return TOKEN_ASSIGN;
	case STATE_SQUAREDBRACKETOPEN:
		return TOKEN_SQUAREDBRACKETOPEN;
	case STATE_SQUAREDBRACKETCLOSE:
		return TOKEN_SQUAREDBRACKETCLOSE;
	case STATE_EQUALS:
		return TOKEN_EQUALS;
	case STATE_IDENTIFIER:
		return TOKEN_IDENTIFIER;
	case STATE_OPENBRACE:
		return TOKEN_OPENBRACE;
	case STATE_CLOSEBRACE:
		return TOKEN_CLOSEBRACE;
	}
	return TOKEN_ILLEGAL;
}

//Public Functions
ESourceToken CSourceLexer::GetNextToken()
{
	EState state, acceptedState;
	ECharClass cc;
	const char *pStart, *pAcceptedPos;
	ESourceToken token;

start:;
	state = STATE_START;
	pStart = this->pSource;

	while(state != STATE_FINISHED)
	{
		if(this->GetTokenType(state))
		{
			acceptedState = state;
			pAcceptedPos = this->pSource;
		}
		cc = this->GetCharClass(*this->pSource);
		state = this->GetNextState(state, cc);
		this->pSource++;
	}

	this->pSource = pAcceptedPos;
	this->tokenValue.Assign(pStart, uint32(this->pSource - pStart));
	token = this->GetTokenType(acceptedState);

	switch(token)
	{
	case TOKEN_END:
		this->pSource--; //Again point to end
		return TOKEN_END;
	case TOKEN_TABSPACE:
	case TOKEN_LINEFEED:
	case TOKEN_CARRIAGERETURN:
		//ignore
		goto start;
	case TOKEN_LITERAL:
		this->tokenValue = this->tokenValue.SubString(1, this->tokenValue.GetLength()-2);
		return TOKEN_LITERAL;
	case TOKEN_SINGLELINECOMMENT:
		{
			char c;

			while(true)
			{
				c = *this->pSource++;

				if(c == '\0')
				{
					this->pSource--;
					return TOKEN_ILLEGAL;
				}
				if(c == '\n')
				{
					goto start; //ignore
				}
			}
		}
	case TOKEN_MULTILINECOMMENT:
		{
			char c;

			while(true)
			{
				c = *this->pSource++;

				if(c == '\0')
				{
					this->pSource--;
					return TOKEN_ILLEGAL;
				}
				if(c == '*' && *this->pSource == '/')
				{
					this->pSource++;
					goto start; //ignore
				}
			}
		}
	case TOKEN_IDENTIFIER:
		if(this->tokenValue == UCS_KEYWORD_ARRAY)
			return TOKEN_ARRAY;
		if(this->tokenValue == UCS_KEYWORD_CONST)
			return TOKEN_CONST;
		if(this->tokenValue == UCS_KEYWORD_FOR)
			return TOKEN_FOR;
		if(this->tokenValue == UCS_KEYWORD_IF)
			return TOKEN_IF;
		if(this->tokenValue == UCS_KEYWORD_INT)
			return TOKEN_INT;
		if(this->tokenValue == UCS_KEYWORD_NEW)
			return TOKEN_NEW;
		if(this->tokenValue == UCS_KEYWORD_RETURN)
			return TOKEN_RETURN;
		if(this->tokenValue == UCS_KEYWORD_VOID)
			return TOKEN_VOID;
		return TOKEN_IDENTIFIER;
	default:
		return token;
	}

	return TOKEN_ILLEGAL;
}

const CString &CSourceLexer::GetTokenValue()
{
	return this->tokenValue;
}

void CSourceLexer::Init(const char *pSource)
{
	this->pSource = pSource;
}