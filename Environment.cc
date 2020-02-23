#include "Environment.h"
#include "Nodes.h"


Environment::Environment() {}

Environment::~Environment() {}

Expression* Environment::read(std::string name)
{
	if (exists(name))
		return variables[name];

	return nullptr;
}

void Environment::write(std::string name, Expression* expression)
{
	if (exists(name))
	{
		if (variables[name]->type != expression->type)
		{
			std::cout << "syntax error: trying to write to variable of different type\n";
			return;
		}
	}

	variables[name] = expression;
}

bool Environment::exists(std::string name)
{
	return variables.count(name) != 0;
}
