#include <iostream>
#include "grammar.tab.hh"
#include "globals.h"


bool debug_lex = false;
bool debug_grammar = false;
bool debug_assignments = false;
bool debug_calls = false;
bool debug_evaluations = false;

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
