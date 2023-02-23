//Global
#include <Windows.h>
//Libs
#include <SJCLib.h>
//Namespaces
using namespace SJCLib;

//Prototypes
bool Assemble(const CString &refInputFileName);
bool Execute(const CString &refInputFileName);
bool EvaluateArguments(CArray<CString> &args);
bool JIT(const CString &refInputFileName);
void PrintManual();

//Definitions
#define APPLICATION_TITLE "RegisterMachine"
#define APPLICATION_NAME APPLICATION_TITLE

//Global Variables
bool g_jit = false;
CString g_Input;

int main(int argc, char* argv[])
{
	bool ok;
	CArray<CString> args;

	SetConsoleTitle(APPLICATION_TITLE);
	stdOut << APPLICATION_TITLE << endl << endl;

	for(uint32 i = 1; i < (uint32)argc; i++)
	{
		args.Push(argv[i]);
	}

	if(EvaluateArguments(args))
	{
		g_Input += ".sjccs";
		if(GetFileExtension(g_Input) == "sjcasm")
		{
			ok = Assemble(g_Input);
		}
		else
		{
			if(g_jit)
				ok = JIT(g_Input);
			else
				ok = Execute(g_Input);
		}

		if(ok)
			return EXIT_SUCCESS;

		return EXIT_FAILURE;
	}
	else
	{
		PrintManual();
		return EXIT_FAILURE;
	}
	
	PrintManual();
	return EXIT_SUCCESS;
}

bool CheckArguments()
{
	return FileExists(g_Input);
}

bool EvaluateArguments(CArray<CString> &args)
{
	//exactly the file must be entered
	if(args.GetNoOfElements() > 2)
		return false;

	if(args[0] == "-jit")
	{
		g_jit = true;
	}
	
	g_Input = args[1];
	
	return CheckArguments();
}

void PrintManual()
{
	stdOut << "Usage: " << endl
		<< "  " << APPLICATION_NAME << " input" << endl << endl
		<< "   input    input file" << endl;
}