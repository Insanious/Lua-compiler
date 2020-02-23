#include <iostream>
#include "grammar.tab.hh"
#include "globals.h"


bool debug_lex = false;
bool debug_grammar = true;
bool debug_assignments = true;
bool debug_calls = true;
bool debug_evaluations = true;

void yy::parser::error(std::string const&err)
{
	std::cout << "It's one of the bad ones... " << err << std::endl;
}

int main(int argc, char **argv)
{
	std::string graph = "";
	yy::parser parser;
	if(!parser.parse())
	{
		root->execute();
		root->createGraphViz();
	}

	return 0;
}
