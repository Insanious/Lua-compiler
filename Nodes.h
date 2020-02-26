#ifndef NODES_H
#define NODES_H

#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <fstream>
#include <cmath>

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
	std::string createLabel(Node* node, int id);
	std::string createConnectionFromTo(int from, int to);

};


class Expression : public Node
{
public:
	enum Type { VARIABLE, STRING, INTEGER, FLOAT, BOOLEAN, PARENTHESIS, BINARYOPERATION, IO_READ, LIST, LENGTH, FUNCTIONCALL } type;

	bool isExecutable;

	Expression();
	Expression(Expression::Type type, bool isExecutable, std::string tag, std::string value);
	virtual Expression* operator == (Expression* obj);
	virtual Expression* operator != (Expression* obj);
	virtual Expression* operator + (Expression* obj);
	virtual Expression* operator - (Expression* obj);
	virtual Expression* operator * (Expression* obj);
	virtual Expression* operator / (Expression* obj);
	virtual Expression* operator ^ (Expression* obj);
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
	AssignmentNode(Environment* environment, std::vector<Expression*> left, std::vector<Expression*> right);
	~AssignmentNode();

	void evaluate();
	Expression* execute();
};


class ListNode : public Expression
{
private:
	std::vector<Expression*> expressions;

public:
	ListNode();
	ListNode(std::vector<Expression*> expressions);
	Expression* operator [] (int index);
	~ListNode();
};


class VariableNode : public Expression
{
private:
	std::string name;
	Environment* environment;

public:
	VariableNode();
	VariableNode(Environment* environment, std::string name);
	VariableNode(Environment* environment, std::string name, Expression* index);
	Expression* operator == (Expression* obj);
	Expression* operator != (Expression* obj);
	Expression* operator + (Expression* obj);
	Expression* operator - (Expression* obj);
	Expression* operator * (Expression* obj);
	Expression* operator / (Expression* obj);
	Expression* operator ^ (Expression* obj);
	~VariableNode();

	void evaluate(Expression*& returnValue);
	void evaluate(std::string& returnValue);

	bool sameType(Expression* other);
};


class LengthNode : public Expression
{
private:
	Expression* variable;

public:
	LengthNode();
	LengthNode(Expression* variable);
	~LengthNode();

	Expression* execute();
};


class IntegerNode : public Expression
{
private:
	int value;

public:
	IntegerNode();
	IntegerNode(int value);
	Expression* operator == (Expression* obj);
	Expression* operator != (Expression* obj);
	Expression* operator + (Expression* obj);
	Expression* operator - (Expression* obj);
	Expression* operator * (Expression* obj);
	Expression* operator / (Expression* obj);
	Expression* operator ^ (Expression* obj);
	~IntegerNode();

	void evaluate(int& returnValue);
};


class FloatNode : public Expression
{
private:
	float value;

public:
	FloatNode();
	FloatNode(float value);
	Expression* operator == (Expression* obj);
	Expression* operator != (Expression* obj);
	Expression* operator + (Expression* obj);
	Expression* operator - (Expression* obj);
	Expression* operator * (Expression* obj);
	Expression* operator / (Expression* obj);
	Expression* operator ^ (Expression* obj);
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
	Expression* operator == (Expression* obj);
	Expression* operator != (Expression* obj);
	Expression* operator + (Expression* obj);
	Expression* operator * (Expression* obj);
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
	Expression* operator == (Expression* obj);
	Expression* operator != (Expression* obj);
	~BooleanNode();

	void evaluate(bool& returnValue);
};


class BinaryOperationNode : public Expression
{
private:
	Expression* left;
	Expression* right;

public:
	enum Operation { EQUALS, NOT_EQUALS, PLUS, MINUS, MULTIPLICATION, DIVISION, MODULUS, POWER_OF, LESS, MORE } operation;

	BinaryOperationNode();
	BinaryOperationNode(Expression* left, Expression* right, BinaryOperationNode::Operation operation);
	~BinaryOperationNode();

	Expression* execute();
};


class ParenthesisNode : public Expression
{
private:
	Expression* expression;

public:
	ParenthesisNode();
	ParenthesisNode(Expression* expression);
	Expression* operator == (Expression* obj);
	Expression* operator != (Expression* obj);
	Expression* operator + (Expression* obj);
	Expression* operator - (Expression* obj);
	Expression* operator * (Expression* obj);
	Expression* operator / (Expression* obj);
	Expression* operator ^ (Expression* obj);
	~ParenthesisNode();

	void evaluate(Expression*& returnValue);
	void evaluate(bool& returnValue);

	Expression* execute();
};


class IOReadNode : public Expression
{
private:
	Expression* variable;

public:
	IOReadNode();
	IOReadNode(Expression* variable);
	IOReadNode(std::string type);
	~IOReadNode();

	Expression* execute();
};


class IOWriteNode : public Statement
{
private:
	std::vector<Expression*> expressions;

public:
	IOWriteNode();
	IOWriteNode(std::vector<Expression*> expressions);
	~IOWriteNode();

	Expression* execute();
};


class ForNode : public Statement
{
private:
	Expression* variable;
	std::vector<Expression*> explist;
	Statement* block;

	Environment* environment;

public:
	ForNode();
	ForNode(Expression* variable, std::vector<Expression*> explist, Statement* block);
	~ForNode();

	Expression* execute();
};


class PrintNode : public Statement
{
private:
	std::vector<Expression*>  expressions;

public:
	PrintNode();
	PrintNode(std::vector<Expression*> expression);
	~PrintNode();

	Expression* execute();
};


class RepeatStatementNode : public Statement
{
private:
	Statement* block;
	Expression* until;

public:
	RepeatStatementNode();
	RepeatStatementNode(Statement* block, Expression* until);
	~RepeatStatementNode();

	Expression* execute();
};


class FunctionCallNode : public Expression, public Statement
{
private:
	Expression* variable;
	std::vector<Expression*> arguments;

public:
	FunctionCallNode();
	FunctionCallNode(Expression* variable, std::vector<Expression*> arguments);
	~FunctionCallNode();

	Expression* execute();
};


class FunctionNode : public Statement
{
private:
	Expression* variable;
	Statement* block;
	std::vector<Expression*> arguments;

public:
	FunctionNode();
	FunctionNode(std::vector<Expression*> arguments);
	FunctionNode(Expression* variable, Statement* block);
	FunctionNode(Expression* variable, Statement* block, std::vector<Expression*> arguments);
	~FunctionNode();

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
	std::vector<Expression*> expressions;

public:
	ReturnNode();
	ReturnNode(std::vector<Expression*> expressions);
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
