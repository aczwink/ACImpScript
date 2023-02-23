//Global
#include <Windows.h>
//SJCLib
#include <SJCLib.h>
//Local
#include "CAssembler.h"
#include "CCompiler.h"
#include "CDisassembler.h"
#include "CRegisterVM.h"
#include "CVM.h"
#include "Machines.h"
#include "resource.h"
//Namespaces
using namespace SJCLib;
//Definitions
#define PROGRAM_ARG_ASM "asm"
#define PROGRAM_ARG_COMP "comp"
#define PROGRAM_ARG_DASM "dasm"
#define PROGRAM_ARG_RUN "run"
#define PROGRAM_ARG_OPTION_DEBUG 'd'
#define PROGRAM_ARG_OPTION_MACHINE 'm'
#define PROGRAM_ARG_MACHINE_STACKMACHINE "sm"
#define PROGRAM_ARG_MACHINE_REGISTERMACHINE "rm"

//Debug Calls
/*
asm -d "D:\Archiv\Coding\Projekte\Fertige Projekte\SJCScript\_Output\Debug_x64\test.sjcasm"
run -d "D:\Archiv\Coding\Projekte\Fertige Projekte\SJCScript\_Output\Debug_x64\test.sjccs"
asm -m rm -d "D:\Archiv\Coding\Projekte\Fertige Projekte\SJCScript\_Output\Debug_x64\sequence 1 div n.sjcasm"
run -m rm -d "D:\Archiv\Coding\Projekte\Fertige Projekte\SJCScript\_Output\Debug_x64\sequence 1 div n.sjccs"
dasm -m rm -d "D:\Archiv\Coding\Projekte\Fertige Projekte\SJCScript\_Output\Debug_x64\1.sjccs"
comp -d "D:\Archiv\Coding\Projekte\Fertige Projekte\SJCScript\_Output\Debug_x64\fib.sjcus"
*/

//Enums
enum ProgramCommand
{
	COMMAND_ILLEGAL,
	COMMAND_ASM,
	COMMAND_COMP,
	COMMAND_DASM,
	COMMAND_RUN
};

//Prototypes
bool AssembleFile(CString &filename, EVirtualMachine vm);
bool CheckArguments();
bool CompileFile(CString input, EVirtualMachine vm);
bool DisassembleFile(CString filename, EVirtualMachine vm);
bool EvaluateArguments(CArray<CString> &args);
void PrintManual();

//Global Variables
ProgramCommand g_Command = COMMAND_ILLEGAL;
CString g_Input;
bool g_DebugMode = false;
EVirtualMachine g_VM = VM_STACK;

int main(int argc, char **argv)
{
	CArray<CString> args;
	
	SetConsoleTitle(APPLICATION_TITLE);
	for(uint32 i = 1; i < (uint32)argc; i++)
	{
		args.Push(argv[i]);
	}

	if(EvaluateArguments(args))
	{
		bool ok = false;

		switch(g_Command)
		{
		case COMMAND_ASM:
			ok = AssembleFile(g_Input, g_VM);
			break;
		case COMMAND_COMP:
			ok = CompileFile(g_Input, g_VM);
			break;
		case COMMAND_DASM:
			ok = DisassembleFile(g_Input, g_VM);
			break;
		case COMMAND_RUN:
			{
				switch(g_VM)
				{
				case VM_STACK:
					{
						CVM vm;
						
						ok = vm.LoadModule(g_Input);
						if(ok)
							vm.Run();
					}
					break;
				case VM_REGISTER:
					{
						CRegisterVM vm;
						
						ok = vm.LoadModule(g_Input);
						if(ok)
							vm.Run();
					}
					break;
				}
			}
			break;
		}

		if(ok)
		{
			stdOut << "Done." << endl;
			return EXIT_SUCCESS;
		}
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

void PrintManual()
{
	stdOut << APPLICATION_NAME << " by " << APPLICATION_CREATOR << " v" << APPLICATION_VERSION_STRING << " Build:" << BUILDDATE_STRING << endl << endl
		<< "Usage: " << endl
		<< "  " << APPLICATION_NAME << " command [options] input" << endl << endl
		<< "   command" << endl
		<< "     " << PROGRAM_ARG_ASM << "      assemble a program" << endl
		<< "     " << PROGRAM_ARG_COMP << "     compile a program" << endl
		<< "     " << PROGRAM_ARG_DASM << "     disassemble a program" << endl
		<< "     " << PROGRAM_ARG_RUN << "      run a program" << endl
		<< "   options" << endl
		<< "     -" << PROGRAM_ARG_OPTION_DEBUG << "        run in debug mode" << endl
		<< "     -" << PROGRAM_ARG_OPTION_MACHINE << " machine" << endl
		<< "   machine" << endl
		<< "     " << PROGRAM_ARG_MACHINE_STACKMACHINE << " - stack machine (default)" << endl
		<< "     " << PROGRAM_ARG_MACHINE_REGISTERMACHINE << " - register machine" << endl
		<< "   input      a input file depending on command" << endl;
}

bool EvaluateArguments(CArray<CString> &args)
{
	//At least 2 args must exist
	if(args.GetLength() < 2)
		return false;

	//First Arg is the command
	if(args[0] == PROGRAM_ARG_ASM)
	{
		g_Command = COMMAND_ASM;
	}
	else if(args[0] == PROGRAM_ARG_COMP)
	{
		g_Command = COMMAND_COMP;
	}
	else if(args[0] == PROGRAM_ARG_DASM)
	{
		g_Command = COMMAND_DASM;
	}
	else if(args[0] == PROGRAM_ARG_RUN)
	{
		g_Command = COMMAND_RUN;
	}
	else
	{
		return false;
	}

	for(uint32 i = 1; i < args.GetLength(); i++)
	{
		if(args[i][0] == '-' && g_Input.IsEmpty()) //An option
		{
			if(args[i][1] == PROGRAM_ARG_OPTION_DEBUG)
			{
				g_DebugMode = true;
			}
			if(args[i][1] == PROGRAM_ARG_OPTION_MACHINE)
			{
				i++;
				if(i >= args.GetLength())
				{
					return false;
				}
				if(args[i] == PROGRAM_ARG_MACHINE_STACKMACHINE)
				{
					g_VM = VM_STACK;
				}
				else if(args[i] == PROGRAM_ARG_MACHINE_REGISTERMACHINE)
				{
					g_VM = VM_REGISTER;
				}
				else
				{
					return false;
				}
			}
		}
		else //Either input or output
		{
			if(g_Input.IsEmpty())
			{
				g_Input = args[i];
			}
			else
			{
				return false;
			}
		}
	}

	return CheckArguments();
}

bool CheckArguments()
{
	return true;
}