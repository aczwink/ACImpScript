//SJCLib
#include <SJCLib.h>
//Global
#include <iostream>
//Local
#include "CASTChecker.h"
#include "CCompiler.h"
//Namespaces
using namespace SJCLib;

bool CompileFile(CString input, EVirtualMachine vm)
{
	CCompiler compiler;
	CParser parser;
	CASTChecker checker;
	CBIFStream file;
	char *pSource;

	if(vm != VM_STACK)
	{
		stdErr << "Error: No compiler is available for this machine." << endl;
		return false;
	}
	
	if(!file.Open(input))
		return false;
	pSource = (char *)malloc(file.GetFileSize()+1);
	file.ReadBytes(pSource, file.GetFileSize());
	pSource[file.GetFileSize()] = '\0';
	
	if(!parser.Parse(pSource))
	{
		stdErr << "Error: Parsing failed." << endl;
		free(pSource);
		return false;
	}
	free(pSource);
	
	checker.Init(parser.GetAbstractSyntaxTree());
	if(!checker.Check())
		return false;
	
	compiler.Init(parser.GetAbstractSyntaxTree(), checker.GetConstants());
	if(!compiler.Compile())
		return false;
	
	ReleaseAST(parser.GetAbstractSyntaxTree());
	return compiler.Output(GetFilePath(input) + "\\" + GetFileName(input) + '.' + COMPILEDSCRIPT_FILEEXT);
}