#pragma once

enum EToken
{
	TOKEN_ILLEGAL,
	TOKEN_END,
	TOKEN_WHITESPACE,
	TOKEN_LINEFEED,
	TOKEN_SINGLELINECOMMENT,
	TOKEN_MULTILINECOMMENT,
	TOKEN_NUMBER,
	TOKEN_LITERAL,
	TOKEN_COMMA,
	TOKEN_PARENCLOSE,
	TOKEN_PARENOPEN,
	TOKEN_REGISTER,
	TOKEN_LABEL,
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD_DATABLOCK,
	TOKEN_KEYWORD_CODEBLOCK,
	TOKEN_KEYWORD_FLOAT32,
	TOKEN_KEYWORD_FLOAT64,
};