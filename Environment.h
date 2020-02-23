#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H


#include <map>

class Expression;

class Environment
{
private:
	std::map<std::string, Expression*> variables;

public:
	Environment();
	~Environment();

	Expression* read(Expression* variable);
	void write(Expression* variable, Expression* expression);
	bool exists(Expression* variable);
};


#endif
