#pragma once
//Libs
#include <SJCLib.h>
//Local
#include "AsmTokens.h"
//Namespaces
using namespace SJCLib;

class CAsmLexer : public CLexer<EToken>
{
private:
	//Methods
	bool OnNextToken(const EToken &refParsedToken);
public:
	//Constructor
	CAsmLexer();
};