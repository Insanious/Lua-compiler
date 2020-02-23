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

	Expression* read(std::string name);
	void write(std::string name, Expression* expression);
	bool exists(std::string name);
};


#endif
