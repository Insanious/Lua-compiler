#include "Environment.h"
#include "Nodes.h"


Environment::Environment() {}

Environment::~Environment() {}

Expression* Environment::read(Expression* variable)
{
	if (exists(variable))
	{
		std::string name = "";
		variable->evaluate(name);

		return variables[name];
	}

	return nullptr;
}

void Environment::write(Expression* variable, Expression* expression)
{
	std::string name = "";
	variable->evaluate(name);

	if (exists(variable))
	{
		if (variables[name]->type != expression->type)
		{
			std::cout << "syntax error: trying to write to variable of different type\n";
			return;
		}
	}

	variables[name] = expression;
}

bool Environment::exists(Expression* variable)
{
	//std::cout << "bool Environment::exists(Expression* variable)\n";
	
	std::string name = "";
	variable->evaluate(name);

	bool doesExist = variables.count(name) != 0;

	/*if (doesExist)
		std::cout << "-Variable does exist\n";
	else
		std::cout << "-Variable does exist\n";*/

	return doesExist;
}
