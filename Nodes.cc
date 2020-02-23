#include "Nodes.h"
#include "Environment.h"
#include "globals.h"


void log_assignments(std::string message)
{
	if (debug_assignments)
		std::cout << message + '\n';
}

void log_calls(std::string message)
{
	if (debug_calls)
		std::cout << message + '\n';
}

void log_evaluations(std::string message)
{
	if (debug_evaluations)
		std::cout << message + '\n';
}


Node::Node()
{
	this->tag = "uninitialised";
	this->value = "uninitialised";
}

Node::Node(std::string tag, std::string value)
{
	this->tag = tag;
	this->value = value;
}

void Node::dump(int depth)
{
	for(int i = 0; i < depth; i++)
		std::cout << "--";
	std::cout << tag << ':' << value << '\n';
	for(int i = 0; i != children.size(); i++)
		children[i]->dump(depth+1);
}

void Node::createGraphViz()
{
	std::queue<Node> nodes;
	int id = 0;
	int parentId = -1;

	std::string graph = "digraph { \n";
	graph += createLabel(*this, id++);
	nodes.push(*this);

	while (nodes.size())
	{
		Node parent = nodes.front();
		nodes.pop();
		parentId++;

		for (int i = 0; i < parent.children.size(); i++)
		{
			nodes.push(*parent.children[i]);
			graph += createLabel(*parent.children[i], id);
			graph += createConnectionFromTo(parentId, id++);
		}
	}

	graph += '}';

	std::ofstream file;
	file.open("graph.dot");
	file << graph;
	file.close();
}

std::string Node::createLabel(Node node, int id)
{
	std::string label = "";
	if (node.value.length())
		label = '\t' + std::to_string(id) + " [label=\"" + node.tag + " = " + node.value + "\"];\n";
	else
		label = '\t' + std::to_string(id) + " [label=\"" + node.tag + "\"];\n";
	return label;
}

std::string Node::createConnectionFromTo(int from, int to)
{
	std::string connection = '\t' + std::to_string(from) + " -> " + std::to_string(to) + "\n";
	return connection;
}



Expression::Expression() {}

Expression::Expression(Expression::Type type, bool isExecutable, std::string tag, std::string value) : Node(tag, value)
{
	this->type = type;
	this->isExecutable = isExecutable;
}

Expression* Expression::operator == (Expression &obj)
{
	log_calls("Expression* Expression::operator == (Expression &obj)");
	return nullptr;
}

Expression* Expression::operator + (Expression &obj)
{
	log_calls("Expression* Expression::operator + (Expression &obj)");
	return nullptr;
}

Expression::~Expression() {}

void Expression::evaluate(std::string& returnValue)
{
	log_calls("void Expression::evaluate(std::string& returnValue)");
}

void Expression::evaluate(int& returnValue)
{
	log_calls("void Expression::evaluate(int& returnValue)");
}

void Expression::evaluate(float& returnValue)
{
	log_calls("void Expression::evaluate(float& returnValue)");
}

void Expression::evaluate(bool& returnValue)
{
	log_calls("void Expression::evaluate(bool& returnValue)");
}

void Expression::evaluate(Expression*& returnValue)
{
	log_calls("void Expression::evaluate(Expression*& returnValue)");
}

Expression* Expression::execute()
{
	log_calls("Expression* Expression::execute()");
	return nullptr;
}

bool Expression::sameType(Expression* other)
{
	log_calls("bool Expression::sameType(Expression* other)");

	return false;
}



Statement::Statement() {}

Statement::Statement(std::string tag, std::string value) : Node(tag, value) {}

Statement::~Statement() {}

void Statement::evaluate(std::string& returnValue)
{
	log_evaluations("void Statement::evaluate(std::string& returnValue)");
}

void Statement::evaluate(int& returnValue)
{
	log_evaluations("void Statement::evaluate(int& returnValue)");
}

void Statement::evaluate(float& returnValue)
{
	log_evaluations("void Statement::evaluate(float& returnValue)");
}

void Statement::evaluate(bool& returnValue)
{
	log_evaluations("bool Statement::evaluate(bool& returnValue)");
}

void Statement::evaluate(Expression*& returnValue)
{
	log_evaluations("bool Statement::evaluate(bool& returnValue)");
}

Expression* Statement::execute()
{
	log_calls("Statement::execute()");
	return nullptr;
}



AssignmentNode::AssignmentNode() : Statement("AssignmentNode", "") {}

AssignmentNode::AssignmentNode(Environment* environment, Expression* left, Expression* right) : Statement("AssignmentNode", "")
{
	log_calls("AssignmentNode::AssignmentNode(Environment* environment, Expression* left, Expression* right)");

	this->children.push_back(left);
	this->children.push_back(right);

	this->environment = environment;
	this->left = left;
	this->right = right;
}

AssignmentNode::~AssignmentNode() {}

void AssignmentNode::evaluate()
{
	log_evaluations("AssignmentNode::evaluate()");
}

Expression* AssignmentNode::execute()
{
	log_calls("Expression* AssignmentNode::execute()");

	if (left->isExecutable)
		left = left->execute();
	if (right->isExecutable)
		right = right->execute();

	if (left->type != Expression::Type::VARIABLE) // Can't assign a non-variable
	{
		std::cout << "syntax error: non-VARIABLE assignment\n";
		return nullptr;
	}

	std::string leftName = "";
	Expression* leftExpression = nullptr;
	Expression* rightExpression = right;
	left->evaluate(leftName);
	left->evaluate(leftExpression);

	if (right->type == Expression::Type::VARIABLE)
	{
		std::string rightName = "";
		right->evaluate(rightName);
		right->evaluate(rightExpression);
		if (rightExpression == nullptr) // Check if right variable exists
		{
			std::cout << "syntax error: trying to assign a var to an undeclared variable\n";
			return nullptr;
		}
	}
	/*else if (right->type == Expression::Type::PLUS)
		right->evaluate(rightExpression);*/



	if (environment->exists(leftName) && !left->sameType(rightExpression)) // Check if type doesn't match
	{
		std::cout << "syntax error: trying to assign a variable with an expression of the wrong type" << leftExpression->type << ", " << rightExpression->type << '\n';
		return nullptr;
	}

	environment->write(leftName, rightExpression);



	/* --- Output the assignment --- */
	std::string output = "";
	switch (rightExpression->type)
	{
		case Expression::Type::VARIABLE:
		{
			rightExpression->evaluate(output);
			output = leftName + " = " + output;
			break;
		}
		case Expression::Type::STRING:
		{
			rightExpression->evaluate(output);
			output = leftName + " = " + output;
			break;
		}
		case Expression::Type::INTEGER:
		{
			int value = 0;
			rightExpression->evaluate(value);
			output = leftName + " = " + std::to_string(value);
			break;
		}
		case Expression::Type::FLOAT:
		{
			float value = 0.0;
			rightExpression->evaluate(value);
			output = leftName + " = " + std::to_string(value);
			break;
		}
		case Expression::Type::BOOLEAN:
		{
			bool value = false;
			rightExpression->evaluate(value);
			if (value)
				output = leftName + " = true";
			else
				output = leftName + " = false";
			break;
		}
		case Expression::Type::EQUALS:
		{
			bool value = false;
			rightExpression->evaluate(value);
			if (value)
				output = leftName + " = true";
			else
				output = leftName + " = false";
			break;
		}
	}

	log_assignments("-ASSIGNMENT-\t\t\t\t\t\t " + output);
}



VariableNode::VariableNode() {}

VariableNode::VariableNode(Environment* environment, std::string name) : Expression(Expression::Type::VARIABLE, false, "VariableNode", name)
{
	log_calls("VariableNode::VariableNode(Environment* environment, std::string name)");

	this->environment = environment;
	this->name = name;
}

Expression* VariableNode::operator == (Expression &obj)
{
	log_calls("Expression* VariableNode::operator == (Expression &obj)");

	return obj == *environment->read(this->name);
}

Expression* VariableNode::operator + (Expression &obj)
{
	log_calls("Expression* VariableNode::operator + (Expression &obj)");

	return obj + *environment->read(this->name);
}

VariableNode::~VariableNode() {}

void VariableNode::evaluate(Expression*& returnValue)
{
	log_evaluations("VariableNode::evaluate(Expression*& returnValue)");

	returnValue = environment->read(name);
	if (returnValue)
		std::cout << "var type: " << returnValue->type << '\n';
}

void VariableNode::evaluate(std::string& returnValue)
{
	log_evaluations("VariableNode::evaluate(std::string& returnValue)\t = " + name);
	returnValue = name;
}

bool VariableNode::sameType(Expression* other)
{
	log_calls("bool VariableNode::sameType(Expression* other)");

	return this->environment->read(name)->type == other->type;
}



IntegerNode::IntegerNode() {}

IntegerNode::IntegerNode(int value) : Expression(Expression::Type::INTEGER, false, "IntegerNode", std::to_string(value))
{
	log_calls("IntegerNode::IntegerNode(int value)");

	this->value = value;
}

Expression* IntegerNode::operator == (Expression &obj)
{
	log_calls("Expression* IntegerNode::operator == (Expression &obj)");

	if (obj.type == Expression::Type::VARIABLE || obj.type == Expression::Type::PARENTHESIS || obj.type == Expression::Type::EQUALS) // Check equality with own equality operator
		return obj == *this;

	if (obj.type != Expression::Type::INTEGER)
	{
		std::cout << "syntax error: different types when checking equality\n";
		return nullptr;
	}

	int objValue = 0;
	obj.evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

Expression* IntegerNode::operator + (Expression &obj)
{
	log_calls("int IntegerNode::operator + (Expression &obj)");

	if (obj.type == Expression::Type::VARIABLE || obj.type == Expression::Type::PARENTHESIS || obj.type == Expression::Type::EQUALS)
		return obj + *this;


	if (obj.type != Expression::Type::INTEGER)
	{
		std::cout << "syntax error: different types when checking equality\n";
		return nullptr;
	}

	int objValue = 0;
	obj.evaluate(objValue);

	return new IntegerNode(this->value + objValue);
}

IntegerNode::~IntegerNode() {}

void IntegerNode::evaluate(int& returnValue)
{
	log_evaluations("IntegerNode::evaluate(int& returnValue)\t\t\t = " + std::to_string(value));
	returnValue = value;
}



FloatNode::FloatNode() {}

FloatNode::FloatNode(float value) : Expression(Expression::Type::FLOAT, false, "FloatNode", std::to_string(value))
{
	log_calls("FloatNode::FloatNode(float value)");

	this->value = value;
}

Expression* FloatNode::operator == (Expression &obj)
{
	log_calls("Expression* FloatNode::operator == (Expression &obj)");

	if (obj.type == Expression::Type::VARIABLE || obj.type == Expression::Type::PARENTHESIS || obj.type == Expression::Type::EQUALS)
		return obj == *this;

	if (obj.type != Expression::Type::FLOAT)
	{
		std::cout << "syntax error: different types when checking equality\n";
		return nullptr;
	}

	float objValue = 0.0;
	obj.evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

Expression* FloatNode::operator + (Expression &obj)
{
	log_calls("Expression* FloatNode::operator + (Expression &obj)");

	if (obj.type == Expression::Type::VARIABLE || obj.type == Expression::Type::PARENTHESIS || obj.type == Expression::Type::EQUALS || obj.type == Expression::Type::PLUS)
		return obj + *this;


	if (obj.type != Expression::Type::FLOAT)
	{
		std::cout << "syntax error: different types when checking equality\n";
		return nullptr;
	}

	float objValue = 0.0;
	obj.evaluate(objValue);

	return new FloatNode(this->value + objValue);
}

FloatNode::~FloatNode() {}

void FloatNode::evaluate(float& returnValue)
{
	log_evaluations("void FloatNode::evaluate(float& returnValue)");

	returnValue = value;
}



StringNode::StringNode() {}

StringNode::StringNode(std::string value) : Expression(Expression::Type::STRING, false, "StringNode", value)
{
	log_calls("StringNode::StringNode(std::string value)");

	this->value = value;
}

Expression* StringNode::operator == (Expression &obj)
{
	log_calls("Expression* StringNode::operator == (Expression &obj)");

	if (obj.type == Expression::Type::VARIABLE || obj.type == Expression::Type::PARENTHESIS || obj.type == Expression::Type::EQUALS)
		return obj == *this;

	if (obj.type != Expression::Type::STRING)
	{
		std::cout << "syntax error: different types when checking equality " << this->type << ", " << obj.type << '\n';
		return nullptr;
	}

	std::string objValue = "";
	obj.evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

Expression* StringNode::operator + (Expression &obj)
{
	log_calls("Expression* StringNode::operator + (Expression &obj)");

	if (obj.type == Expression::Type::VARIABLE || obj.type == Expression::Type::PARENTHESIS || obj.type == Expression::Type::EQUALS)
		return obj + *this;


	if (obj.type != Expression::Type::STRING)
	{
		std::cout << "syntax error: different types when checking equality\n";
		return nullptr;
	}

	std::string objValue = "";
	obj.evaluate(objValue);
	std::cout << this->value << ", " << objValue << '\n';
	return new StringNode(this->value + objValue);
}

StringNode::~StringNode() {}

void StringNode::evaluate(std::string& returnValue)
{
	log_evaluations("StringNode::evaluate(std::string& returnValue)");

	returnValue = value;
}



BooleanNode::BooleanNode() {}

BooleanNode::BooleanNode(bool value) : Expression(Expression::Type::BOOLEAN, false, "BooleanNode", std::to_string(value))
{
	log_calls("BooleanNode::BooleanNode(bool value)");

	this->value = value;
}

Expression* BooleanNode::operator == (Expression &obj)
{
	log_calls("Expression* BooleanNode::operator == (Expression &obj)");

	if (obj.type == Expression::Type::VARIABLE) // Check equality with VariableNodes own equality operator
		return obj == *this;

	if (obj.type != Expression::Type::BOOLEAN)
	{
		std::cout << "syntax error: different types when checking equality\n";
		return nullptr;
	}

	bool objValue = false;
	obj.evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

BooleanNode::~BooleanNode() {}

void BooleanNode::evaluate(bool& returnValue)
{
	if (value)
		log_evaluations("BooleanNode::evaluate(bool& returnValue)\t\t = true");
	else
		log_evaluations("BooleanNode::evaluate(bool& returnValue)\t\t = false");

	returnValue = value;
}



BinaryOperationNode::BinaryOperationNode() {}

BinaryOperationNode::BinaryOperationNode(Expression* left, Expression* right, BinaryOperationNode::Operation operation) : Expression(Expression::Type::BINARYOPERATION, true, "BinaryOperationNode", "")
{
	log_calls("BinaryOperationNode::BinaryOperationNode(Expression* left, Expression* right, BinaryOperationNode::Operation operation)");

	this->children.push_back(left);
	this->children.push_back(right);

	this->left = left;
	this->right = right;
	this->operation = operation;

	switch(this->operation)
	{
		case BinaryOperationNode::Operation::EQUALS:
			this->value = "==";
			break;
		case BinaryOperationNode::Operation::PLUS:
			this->value = "+";
			break;
		case BinaryOperationNode::Operation::MINUS:
			this->value = "-";
			break;
	}
}

BinaryOperationNode::~BinaryOperationNode() {}

Expression* BinaryOperationNode::execute()
{
	log_calls("Expression* BinaryOperationNode::execute()");

	if (left->isExecutable)
		left = left->execute();
	std::cout << "huh\n";
	if (right->isExecutable)
		right = right->execute();
	std::cout << "operation: " << this->operation << '\n';

		switch(this->operation)
		{
			case BinaryOperationNode::Operation::EQUALS:
				return *left == *right;
			case BinaryOperationNode::Operation::PLUS:
				std::cout << "huh\n";
				return *left + *right;
			//case BinaryOperationNode::Operation::MINUS:
				//return *left - *right;
		}
}



EqualsNode::EqualsNode() {}

EqualsNode::EqualsNode(Expression* left, Expression* right) : Expression(Expression::Type::EQUALS, true, "EqualsNode", "")
{
	log_calls("EqualsNode::EqualsNode(Expression* left, Expression* right)");

	this->children.push_back(left);
	this->children.push_back(right);

	this->left = left;
	this->right = right;
}

EqualsNode::~EqualsNode() {}

void EqualsNode::evaluate(bool& returnValue)
{
	log_evaluations("void EqualsNode::evaluate(bool& returnValue)");

	returnValue = *left == *right;
}

Expression* EqualsNode::execute()
{
	log_calls("Expression* EqualsNode::execute()");
	if (left->isExecutable)
		left = left->execute();
	if (right->isExecutable)
		right = right->execute();

	return new BooleanNode(*right == *left);
}



PlusNode::PlusNode() {}

PlusNode::PlusNode(Expression* left, Expression* right) : Expression(Expression::Type::PLUS, true, "PlusNode", "")
{
	log_calls("PlusNode::PlusNode(Expression* left, Expression* right)");

	this->children.push_back(left);
	this->children.push_back(right);

	this->left = left;
	this->right = right;
}

void PlusNode::evaluate(Expression*& returnValue)
{
	log_evaluations("void PlusNode::evaluate(Expression*& returnValue)");

	returnValue = *left + *right;
}

Expression* PlusNode::execute()
{
	log_calls("Expression* PlusNode::execute()");

	if (left->isExecutable)
		left = left->execute();
	if (right->isExecutable)
		right = right->execute();

	return *left + *right;
}



ParenthesisNode::ParenthesisNode() {}

ParenthesisNode::ParenthesisNode(Expression* expression) : Expression(Expression::Type::PARENTHESIS, true, "ParenthesisNode", "")
{
	log_calls("ParenthesisNode::ParenthesisNode(Expression* expression)");
	this->children.push_back(expression);

	this->expression = expression;
}

ParenthesisNode::~ParenthesisNode() {}

Expression* ParenthesisNode::operator == (Expression& obj)
{
	log_calls("Expression* ParenthesisNode::operator == (Expression &obj)");

	// Reverse equality because the ParenthesisNode has been untangled through its expression, no longer a ParenthesisNode
	return *this->expression == obj;
}

Expression* ParenthesisNode::operator + (Expression& obj)
{
	log_calls("Expression* ParenthesisNode::operator + (Expression& obj)");
	return *this->expression + obj;
}

void ParenthesisNode::evaluate(Expression*& returnValue)
{
	log_evaluations("ParenthesisNode::evaluate(Expression*& returnValue)");

	returnValue = this->expression;
}

void ParenthesisNode::evaluate(bool& returnValue)
{
	log_evaluations("ParenthesisNode::evaluate(Expression*& returnValue)");

	this->expression->evaluate(returnValue);
}

Expression* ParenthesisNode::execute()
{
	if (this->expression->isExecutable)
		this->expression = this->expression->execute();

	return this->expression;
}



IfStatementNode::IfStatementNode() : Statement("IfStatementNode", "") {}

IfStatementNode::IfStatementNode(std::vector<Statement*> ifNodes) : Statement("IfStatementNode", "")
{
	log_calls("IfStatementNode::IfStatementNode(std::vector<Statement*> ifNodes)");

	for (auto ifNode : ifNodes)
		this->children.push_back(ifNode);

	this->ifNodes = ifNodes;
}

IfStatementNode::~IfStatementNode() {}

void IfStatementNode::evaluate()
{
	log_evaluations("void IfStatementNode::evaluate()");
}

Expression* IfStatementNode::execute()
{
	log_calls("Expression* IfStatementNode::execute()");

	for (auto ifNode : ifNodes)
	{
		bool returnValue = false;
		ifNode->evaluate(returnValue);
		if (returnValue)
			return ifNode->execute();
	}
}



IfNode::IfNode() : Statement("IfNode", "") {}

IfNode::IfNode(Expression* expression, Statement* block) : Statement("IfNode", "")
{
	log_calls("IfNode::IfNode(Expression* expression, Statement* block)");

	this->children.push_back(expression);
	this->children.push_back(block);

	this->expression = expression;
	this->block = block;
}

IfNode::~IfNode() {}

void IfNode::evaluate(bool& returnValue)
{
	log_evaluations("void IfNode::evaluate(bool& returnValue)");

	if (expression->isExecutable)
		expression = expression->execute();

	expression->evaluate(returnValue);
}

Expression* IfNode::execute()
{
	log_calls("Expression* IfNode::execute()");

	return block->execute();
}



ElseNode::ElseNode() : Statement("ElseNode", "")
{
	this->block = nullptr;
}

ElseNode::ElseNode(Statement* block) : Statement("ElseNode", "")
{
	log_calls("ElseNode::ElseNode(Statement* block)");

	this->children.push_back(block);
	this->block = block;
}

ElseNode::~ElseNode() {}

void ElseNode::evaluate(bool& returnValue)
{
	log_evaluations("void ElseNode::evaluate(bool& returnValue)");

	returnValue = true;
}

Expression* ElseNode::execute()
{
	log_calls("Expression* ElseNode::execute()");

	if (block != nullptr)
		return block->execute();
	return nullptr;
}



LastStatement::LastStatement() : Statement("LastStatement", "") {}

LastStatement::~LastStatement() {}

void LastStatement::evaluate()
{
	log_evaluations("void LastStatement::evaluate()");
}

Expression* LastStatement::execute()
{
	log_calls("Expression* LastStatement::execute()");
	return nullptr;
}



ReturnNode::ReturnNode() : Statement("ReturnNode", "") {}

ReturnNode::ReturnNode(Expression* expression) : Statement("ReturnNode", "")
{
	log_calls("ReturnNode::ReturnNode(Expression* expression)");

	this->children.push_back(expression);
	this->expression = expression;
}

ReturnNode::~ReturnNode() {}

void ReturnNode::evaluate()
{
	log_evaluations("void ReturnNode::evaluate()");
}



BreakNode::BreakNode() : Statement("BreakNode", "") {}

BreakNode::~BreakNode() {}

void BreakNode::evaluate()
{
	log_evaluations("void BreakNode::evaluate()");
}



SemicolonNode::SemicolonNode() : Statement("SemicolonNode", "")
{
	exists = false;
}

SemicolonNode::SemicolonNode(std::string semi) : Statement("SemicolonNode", "")
{
	log_calls("SemicolonNode::SemicolonNode(std::string semi)");

	exists = true;
}

SemicolonNode::~SemicolonNode() {}

void SemicolonNode::evaluate()
{
	log_evaluations("void SemicolonNode::evaluate()");
}



Chunk::Chunk() : Statement("Chunk", "") {}

Chunk::~Chunk() {}

void Chunk::evaluate()
{
	log_evaluations("void Chunk::evaluate()");
}

Expression* Chunk::execute()
{
	log_calls("Expression* Chunk::execute()");
	return nullptr;
}



Block::Block() : Statement("Block", "") {}

Block::Block(std::vector<Statement*> statements) : Statement("Block", "")
{
	log_calls("Block::Block(std::vector<Statement*> statements)");

	for(auto statement : statements)
		this->children.push_back(statement);

	this->statements = statements;
}

Block::~Block() {}

void Block::evaluate()
{
	log_evaluations("void Block::evaluate()");
}

Expression* Block::execute()
{
	log_calls("Expression* Block::execute()");

	for(auto statement : statements)
		Expression* res = statement->execute();
}
