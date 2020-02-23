#ifndef NODES_H
#define NODES_H

#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <fstream>

class Environment;

class Node
{
public:
	std::string tag, value;
	std::vector<Node*> children;
	Node(std::string t, std::string v);
	Node();

	void dump(int depth=0);
	void createGraphViz();
	std::string createLabel(Node node, int id);
	std::string createConnectionFromTo(int from, int to);

};


class Expression : public Node
{
public:
	enum Type { VARIABLE, STRING, INTEGER, FLOAT, BOOLEAN, EQUALS, PARENTHESIS, PLUS, BINARYOPERATION } type;

	bool isExecutable;

	Expression();
	Expression(Expression::Type type, bool isExecutable, std::string tag, std::string value);
	virtual Expression* operator == (Expression &obj);
	virtual Expression* operator + (Expression &obj);
	~Expression();

	virtual void evaluate(std::string& returnValue);
	virtual void evaluate(int& returnValue);
	virtual void evaluate(float& returnValue);
	virtual void evaluate(bool& returnValue);
	virtual void evaluate(Expression*& returnValue);

	virtual Expression* execute();

	virtual bool sameType(Expression* other);
};


class Statement : public Node
{
public:
	Statement();
	Statement(std::string tag, std::string value);
	~Statement();

	virtual void evaluate(std::string& returnValue);
	virtual void evaluate(int& returnValue);
	virtual void evaluate(float& returnValue);
	virtual void evaluate(bool& returnValue);
	virtual void evaluate(Expression*& returnValue);

	virtual Expression* execute();
};


class AssignmentNode : public Statement
{
private:
	Environment* environment;
	Expression* left;
	Expression* right;

public:
	AssignmentNode();
	AssignmentNode(Environment* environment, Expression* left, Expression* right);
	~AssignmentNode();

	void evaluate();
	Expression* execute();
};


class VariableNode : public Expression
{
private:
	std::string name;
	Environment* environment;

public:
	VariableNode();
	VariableNode(Environment* environment, std::string name);
	Expression* operator == (Expression &obj);
	Expression* operator + (Expression &obj);
	~VariableNode();

	void evaluate(Expression*& returnValue);
	void evaluate(std::string& returnValue);

	bool sameType(Expression* other);
};


class IntegerNode : public Expression
{
private:
	int value;

public:
	IntegerNode();
	IntegerNode(int value);
	Expression* operator == (Expression &obj);
	Expression* operator + (Expression &obj);
	~IntegerNode();

	void evaluate(int& returnValue);
	Expression* right;
};


class FloatNode : public Expression
{
private:
	float value;

public:
	FloatNode();
	FloatNode(float value);
	Expression* operator == (Expression &obj);
	Expression* operator + (Expression &obj);
	~FloatNode();

	void evaluate(float& returnValue);
};


class StringNode : public Expression
{
private:
	std::string value;

public:
	StringNode();
	StringNode(std::string value);
	Expression* operator == (Expression &obj);
	Expression* operator + (Expression &obj);
	~StringNode();

	void evaluate(std::string& returnValue);
};


class BooleanNode : public Expression
{
private:
	bool value;

public:
	BooleanNode();
	BooleanNode(bool value);
	Expression* operator == (Expression &obj);
	~BooleanNode();

	void evaluate(bool& returnValue);
};


class BinaryOperationNode : public Expression
{
private:
	Expression* left;
	Expression* right;

public:
	enum Operation { EQUALS, PLUS, MINUS } operation;

	BinaryOperationNode();
	BinaryOperationNode(Expression* left, Expression* right, BinaryOperationNode::Operation operation);
	~BinaryOperationNode();

	Expression* execute();
};


class EqualsNode : public Expression
{
private:
	Expression* left;
	Expression* right;

public:
	EqualsNode();
	EqualsNode(Expression* left, Expression* right);
	~EqualsNode();

	void evaluate(bool& returnValue);
	Expression* execute();
};


class PlusNode : public Expression
{
private:
	Expression* left;
	Expression* right;

public:
	PlusNode();
	PlusNode(Expression* left, Expression* right);
	~PlusNode();

	void evaluate(Expression*& returnValue);

	Expression* execute();
};


class ParenthesisNode : public Expression
{
private:
	Expression* expression;

public:
	ParenthesisNode();
	ParenthesisNode(Expression* expression);
	Expression* operator == (Expression& obj);
	Expression* operator + (Expression& obj);
	~ParenthesisNode();

	void evaluate(Expression*& returnValue);
	void evaluate(bool& returnValue);

	Expression* execute();
};


class IfStatementNode : public Statement
{
private:
	std::vector<Statement*> ifNodes;

public:
	IfStatementNode();
	IfStatementNode(std::vector<Statement*> ifNodes);
	~IfStatementNode();

	void evaluate();
	Expression* execute();
};


class IfNode : public Statement
{
private:
	Statement* block;
	Expression* expression;

public:
	IfNode();
	IfNode(Expression* expression, Statement* block);
	~IfNode();

	void evaluate(bool& returnValue);
	Expression* execute();
};


class ElseNode : public Statement
{
private:
	Statement* block;

public:
	ElseNode();
	ElseNode(Statement* block);
	~ElseNode();

	void evaluate(bool& returnValue);
	Expression* execute();
};



class LastStatement : public Statement
{
private:

public:
	LastStatement();
	~LastStatement();

	void evaluate();
	Expression* execute();
};


class ReturnNode : public Statement
{
private:
	Expression* expression;

public:
	ReturnNode();
	ReturnNode(Expression* expression);
	~ReturnNode();

	void evaluate();
};


class BreakNode : public Statement
{
private:

public:
	BreakNode();
	~BreakNode();

	void evaluate();
};


class SemicolonNode : public Statement
{
private:
	bool exists;

public:
	SemicolonNode();
	SemicolonNode(std::string semi);
	~SemicolonNode();

	void evaluate();
};



class Chunk : public Statement
{
private:

public:

	Chunk();
	~Chunk();

	void evaluate();
	Expression* execute();
};


class Block : public Statement
{
private:
	std::vector<Statement*> statements;

public:
	Block();
	Block(std::vector<Statement*> statements);
	~Block();

	void evaluate();
	Expression* execute();
};




#endif
