//Class Header
#include "CAsmLexer.h"

//Constructor
CAsmLexer::CAsmLexer()
{
	this->pSource = NULL;
}

//Destructor
CAsmLexer::~CAsmLexer()
{
}

//Private Functions
CharClass CAsmLexer::EvaluateChar(char c)
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
	case 35: //#
		return CC_NUMBERSIGN;
	case 43: //+
		return CC_PLUS;
	case 44: //,
		return CC_COMMA;
	case 45: //-
		return CC_MINUS;
	case 46: //.
		return CC_DOT;
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
	case 58: //:
		return CC_COLON;
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
	case 95: //_
		return CC_UNDERSCORE;
	case 33: //!
	case 40: //(
	case 41: //)
	case 47: ///
	case 61: //=
	case 92: /* \ */
	case 94: //^
	case 123: //{
	case 125: //}
		return CC_INVALID;
	/*
	case 63: //?
		return CHAR_QUESTIONMARK;
	case 64: //@
		return CHAR_AT;
	case 91: //[
		return CHAR_SQUAREDBRACKETOPEN;
	case 93: //]
		return CHAR_SQUAREDBRACKETCLOSE;
	case 124: //|
		return CHAR_OR;
	case 126: //~
		return CHAR_TILDE;*/
	/*
	case 42: //*
		return CHAR_MULTIPLY;
	case 36: //$
	case 39: //'
		return CHAR_INVALID;
		/*
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 11:
		case 12:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 96: //`
		case 127: //DEL
			return CHAR_ILLEGAL;
		case 37: //%
			return CHAR_MODULO;
		case 60: //<
			return CHAR_SMALLERTHAN;
		case 62: //>
			return CHAR_GREATERTHAN;
		*/
		/*
	case 38: //&
	case 59: //;
		*/
	}
	return CC_ILLEGAL;
}

Token CAsmLexer::GetTokenType(TokenState state)
{
	switch(state)
	{
	case STATE_END:
		return TOKEN_END;
	case STATE_TABSPACE:
		return TOKEN_TABSPACE;
	case STATE_CARRIAGERETURN:
		return TOKEN_CARRIAGERETURN;
	case STATE_LINEFEED:
		return TOKEN_LINEFEED;
	case STATE_LITERAL:
		return TOKEN_LITERAL;
	case STATE_NUMBERSIGN:
		return TOKEN_COMMENT;
	case STATE_COMMA:
		return TOKEN_COMMA;
	case STATE_NUMBER:
		return TOKEN_NUMBER;
	case STATE_FLOAT:
		return TOKEN_FLOAT;
	case STATE_LETTER:
		return TOKEN_IDENTIFIER;
	case STATE_LABEL:
		return TOKEN_LABEL;
	}
	return TOKEN_ILLEGAL;
}

TokenState CAsmLexer::NextState(TokenState state, CharClass c)
{
	switch(state)
	{
	case STATE_START:
		switch(c)
		{
		case CC_NULLBYTE:
			return STATE_END;
		case CC_TABSPACE:
			return STATE_TABSPACE;
		case CC_CARRIAGERETURN:
			return STATE_CARRIAGERETURN;
		case CC_INVERTEDCOMMA:
			return STATE_UNCLOSEDLITERAL;
		case CC_LINEFEED:
			return STATE_LINEFEED;
		case CC_NUMBERSIGN:
			return STATE_NUMBERSIGN;
		case CC_PLUS:
			return STATE_PLUS;
		case CC_COMMA:
			return STATE_COMMA;
		case CC_MINUS:
			return STATE_MINUS;
		case CC_NUMBER:
			return STATE_NUMBER;
		case CC_LETTER:
			return STATE_LETTER;
		}
		break;
	case STATE_TABSPACE:
		switch(c)
		{
		case CC_TABSPACE:
			return STATE_TABSPACE;
		case CC_CARRIAGERETURN:
		case CC_INVERTEDCOMMA:
		case CC_NUMBERSIGN:
		case CC_NUMBER:
		case CC_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_CARRIAGERETURN:
		switch(c)
		{
		case CC_LINEFEED:
		case CC_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_LINEFEED:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
		case CC_NUMBERSIGN:
		case CC_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_UNCLOSEDLITERAL:
		switch(c)
		{
		case CC_TABSPACE:
		case CC_COMMA:
		case CC_NUMBER:
		case CC_LETTER:
		case CC_COLON:
		case CC_INVALID:
			return STATE_UNCLOSEDLITERAL;
		case CC_INVERTEDCOMMA:
			return STATE_LITERAL;
		}
		break;
	case STATE_LITERAL:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_CARRIAGERETURN:
			return STATE_FINISHED;
			break;
		}
		break;
	case STATE_NUMBERSIGN:
		switch(c)
		{
		case CC_TABSPACE:
		case CC_NUMBER:
		case CC_LETTER:
		case CC_INVALID:
			return STATE_FINISHED;
		}
		break;
	case STATE_PLUS:
		switch(c)
		{
		case CC_NUMBER:
			return STATE_NUMBER;
		}
		break;
	case STATE_COMMA:
		switch(c)
		{
		case CC_TABSPACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_MINUS:
		switch(c)
		{
		case CC_NUMBER:
			return STATE_NUMBER;
		}
		break;
	case STATE_NUMBER:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
		case CC_COMMA:
			return STATE_FINISHED;
		case CC_NUMBER:
			return STATE_NUMBER;
		case CC_DOT:
			return STATE_UNFINISHEDFLOAT;
		}
		break;
	case STATE_UNFINISHEDFLOAT:
		switch(c)
		{
		case CC_NUMBER:
			return STATE_FLOAT;
		}
		break;
	case STATE_FLOAT:
		switch(c)
		{
		case CC_CARRIAGERETURN:
			return STATE_FINISHED;
		case CC_NUMBER:
			return STATE_FLOAT;
		}
		break;
	case STATE_LETTER:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
		case CC_PLUS:
		case CC_COMMA:
		case CC_MINUS:
			return STATE_FINISHED;
		case CC_NUMBER:
		case CC_LETTER:
		case CC_UNDERSCORE:
			return STATE_LETTER;
		case CC_COLON:
			return STATE_LABEL;
		}
		break;
	case STATE_LABEL:
		switch(c)
		{
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
			return STATE_FINISHED;
		}
		break;
	}
	//this should never happen
	return STATE_ILLEGAL;
}

//Public Functions
uint32 CAsmLexer::GetCurrentLineNumber() const
{
	return this->currentLineNumber;
}

CString CAsmLexer::GetCurrentTokenValue()
{
	return this->tokenValue;
}

void CAsmLexer::Init(const char *pSource)
{
	this->pSource = (char *)pSource;
	this->currentLineNumber = 1;
}

Token CAsmLexer::GetNextToken()
{
	char *pStart;
	TokenState state;
	TokenState acceptedState;
	char *pAcceptedPos;
	Token t;
	CharClass c;

start:
	pStart = this->pSource;
	state = STATE_START;

	while(state != STATE_FINISHED)
	{
		if(this->GetTokenType(state))
		{
			acceptedState = state;
			pAcceptedPos = this->pSource;

			if(state == STATE_END) break;
		}
		c = this->EvaluateChar(*this->pSource);
		state = this->NextState(state, c);
		this->pSource++;
	}
	this->pSource = pAcceptedPos;
	state = acceptedState;

	this->tokenValue.Assign(pStart, (uint32)(this->pSource - pStart));

	t = this->GetTokenType(state);
	switch(t)
	{
	case TOKEN_LINEFEED:
		this->currentLineNumber++;
	case TOKEN_TABSPACE:
	case TOKEN_CARRIAGERETURN:
		//ignore
		goto start;
	case TOKEN_COMMENT:
		while(*this->pSource != '\n' && *this->pSource != 0) this->pSource++;
		if(*this->pSource != 0)
		{
			this->pSource++;
		}
		//ignore
		goto start;
	case TOKEN_IDENTIFIER:
		return GetKeywordToken(this->tokenValue);
	case TOKEN_END:
		this->pSource--; //again points to the 0 byte so that any following call (which should not happen) to this function again returns TOKEN_END
	default:
		return t;
	}
	return TOKEN_ILLEGAL;
}