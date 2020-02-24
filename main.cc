#include <iostream>
#include "grammar.tab.hh"
#include "globals.h"


bool debug_lex = false;
bool debug_grammar = false;
bool debug_assignments = true;
bool debug_calls = true;
bool debug_evaluations = false;

void yy::parser::error(std::string const&err)
{
	std::cout << "It's one of the bad ones... " << err << std::endl;
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		std::string argument = argv[1];
		if (argument == "nodebug")
		{
			debug_lex = false;
			debug_grammar = false;
			debug_assignments = false;
			debug_calls = false;
			debug_evaluations = false;
		}
	}


	std::string graph = "";
	yy::parser parser;
	if(!parser.parse())
	{
		root->execute();
		root->createGraphViz();
	}

	return 0;
}
