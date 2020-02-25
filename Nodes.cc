#include "Nodes.h"
#include "Environment.h"
#include "globals.h"


void log_assignments(std::string message)
{
	if (debug_assignments)
		std::cout << "-ASSIGNMENT-\t\t\t\t\t\t\t\t " + message + '\n';
}

void log_calls(std::string message)
{
	if (debug_calls)
		std::cout << "CALL:\t\t " + message + '\n';
}

void log_evaluations(std::string message)
{
	if (debug_evaluations)
		std::cout << "EVALUATION:\t " + message + '\n';
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
	for(long unsigned int i = 0; i != children.size(); i++)
		children[i]->dump(depth+1);
}

void Node::createGraphViz()
{
	std::queue<Node*> nodes;
	int id = 0;
	int parentId = -1;

	std::string graph = "digraph { \n";
	graph += createLabel(this, id++);
	nodes.push(this);

	while (nodes.size())
	{
		Node* parent = nodes.front();
		nodes.pop();
		parentId++;

		for (auto child : parent->children)
		{
			nodes.push(child);
			graph += createLabel(child, id);
			graph += createConnectionFromTo(parentId, id++);
		}
	}

	graph += '}';

	std::ofstream file;
	file.open("graph.dot");
	file << graph;
	file.close();
}

std::string Node::createLabel(Node* node, int id)
{
	std::string label = "";
	if (node->value.length())
		label = '\t' + std::to_string(id) + " [label=\"" + node->tag + " = " + node->value + "\"];\n";
	else
		label = '\t' + std::to_string(id) + " [label=\"" + node->tag + "\"];\n";
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

Expression* Expression::operator == (Expression* obj)
{
	log_calls("Expression* Expression::operator == (Expression* obj)");
	return nullptr;
}

Expression* Expression::operator != (Expression* obj)
{
	log_calls("Expression* Expression::operator != (Expression* obj)");
	return nullptr;
}

Expression* Expression::operator + (Expression* obj)
{
	log_calls("Expression* Expression::operator + (Expression* obj)");
	return nullptr;
}

Expression* Expression::operator - (Expression* obj)
{
	log_calls("Expression* Expression::operator - (Expression* obj)");
	return nullptr;
}

Expression* Expression::operator * (Expression* obj)
{
	log_calls("Expression* Expression::operator * (Expression* obj)");
	return nullptr;
}

Expression* Expression::operator / (Expression* obj)
{
	log_calls("Expression* Expression::operator / (Expression* obj)");
	return nullptr;
}

Expression* Expression::operator ^ (Expression* obj)
{
	log_calls("Expression* Expression::operator ^ (Expression* obj)");
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
		std::cout << "SYNTAX ERROR: non-VARIABLE assignment\n";
		return nullptr;
	}

	Expression* rightExpression = right;

	if (right->type == Expression::Type::VARIABLE)
	{
		right->evaluate(rightExpression);
		if (!rightExpression) // Check if right variable exists
		{
			std::cout << "SYNTAX ERROR: trying to assign a var to an undeclared variable\n";
			return nullptr;
		}
	}


	if (environment->exists(left)) // Check if type doesn't match
	{
		if (!left->sameType(rightExpression))
		{
			Expression* leftExpression = nullptr;
			left->evaluate(leftExpression);
			Expression::Type lType = leftExpression->type;
			Expression::Type rType = rightExpression->type;

			// To allow int = float and float = int
			if (!((lType == Expression::Type::INTEGER && rType == Expression::Type::FLOAT) || (lType == Expression::Type::FLOAT && rType == Expression::Type::INTEGER)))
			{
				std::cout << "SYNTAX ERROR: trying to assign a variable with an expression of the wrong type" << lType << ", " << rType << '\n';
				return nullptr;
			}
		}
	}


	environment->write(left, rightExpression);

	/* --- Output the assignment --- */
	std::string leftName = "";
	std::string output = "";

	left->evaluate(leftName);


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
		case Expression::Type::PARENTHESIS:
		case Expression::Type::BINARYOPERATION:
			break;
	}

	log_assignments(output);

	return nullptr;
}



VariableNode::VariableNode() {}

VariableNode::VariableNode(Environment* environment, std::string name) : Expression(Expression::Type::VARIABLE, false, "VariableNode", name)
{
	log_calls("VariableNode::VariableNode(Environment* environment, std::string name)");

	this->environment = environment;
	this->name = name;
}

Expression* VariableNode::operator == (Expression* obj)
{
	log_calls("Expression* VariableNode::operator == (Expression* obj)");

	Expression* objExpression = obj;

	if (obj->type == Expression::Type::VARIABLE)
		obj->evaluate(objExpression);

	return *environment->read(this) == objExpression;
}

Expression* VariableNode::operator != (Expression* obj)
{
	log_calls("Expression* VariableNode::operator != (Expression* obj)");

	Expression* objExpression = obj;

	if (obj->type == Expression::Type::VARIABLE)
		obj->evaluate(objExpression);

	return *environment->read(this) != objExpression;
}

Expression* VariableNode::operator + (Expression* obj)
{
	log_calls("Expression* VariableNode::operator + (Expression* obj)");

	Expression* objExpression = obj;

	if (obj->type == Expression::Type::VARIABLE)
		obj->evaluate(objExpression);

	return *environment->read(this) + objExpression;
}

Expression* VariableNode::operator - (Expression* obj)
{
	log_calls("Expression* VariableNode::operator - (Expression* obj)");

	Expression* objExpression = obj;

	if (obj->type == Expression::Type::VARIABLE)
		obj->evaluate(objExpression);

	return *environment->read(this) - objExpression;
}

Expression* VariableNode::operator * (Expression* obj)
{
	log_calls("Expression* VariableNode::operator * (Expression* obj)");

	Expression* objExpression = obj;

	if (obj->type == Expression::Type::VARIABLE)
		obj->evaluate(objExpression);

	return *environment->read(this) * objExpression;
}

Expression* VariableNode::operator / (Expression* obj)
{
	log_calls("Expression* VariableNode::operator / (Expression* obj)");

	Expression* objExpression = obj;

	if (obj->type == Expression::Type::VARIABLE)
		obj->evaluate(objExpression);

	return *environment->read(this) / objExpression;
}

Expression* VariableNode::operator ^ (Expression* obj)
{
	log_calls("Expression* VariableNode::operator ^ (Expression* obj)");

	Expression* objExpression = obj;

	if (obj->type == Expression::Type::VARIABLE)
		obj->evaluate(objExpression);

	return *environment->read(this) ^ objExpression;
}

VariableNode::~VariableNode() {}

void VariableNode::evaluate(Expression*& returnValue)
{
	log_evaluations("VariableNode::evaluate(Expression*& returnValue)");

	returnValue = environment->read(this);
}

void VariableNode::evaluate(std::string& returnValue)
{
	log_evaluations("VariableNode::evaluate(std::string& returnValue)\t = " + name);
	returnValue = name;
}

bool VariableNode::sameType(Expression* other)
{
	log_calls("bool VariableNode::sameType(Expression* other)");

	return this->environment->read(this)->type == other->type;
}



IntegerNode::IntegerNode() {}

IntegerNode::IntegerNode(int value) : Expression(Expression::Type::INTEGER, false, "IntegerNode", std::to_string(value))
{
	log_calls("IntegerNode::IntegerNode(int value)");

	this->value = value;
}

Expression* IntegerNode::operator == (Expression* obj)
{
	log_calls("Expression* IntegerNode::operator == (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);

	if (objExpression->type != Expression::Type::INTEGER && objExpression->type != Expression::Type::FLOAT)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::FLOAT)
	{
		float objValue = 0;
		objExpression->evaluate(objValue);

		return new BooleanNode((float)this->value == objValue);
	}

	int objValue = 0;
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

Expression* IntegerNode::operator != (Expression* obj)
{
	log_calls("Expression* IntegerNode::operator != (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);

	if (objExpression->type != Expression::Type::INTEGER && objExpression->type != Expression::Type::FLOAT)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::FLOAT)
	{
		float objValue = 0;
		objExpression->evaluate(objValue);

		return new BooleanNode((float)this->value != objValue);
	}

	int objValue = 0;
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value != objValue);
}

Expression* IntegerNode::operator + (Expression* obj)
{
	log_calls("Expression* IntegerNode::operator + (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::INTEGER && objExpression->type != Expression::Type::FLOAT)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::FLOAT)
	{
		float objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode((float)this->value + objValue);
	}

	int objValue = 0;
	objExpression->evaluate(objValue);

	return new IntegerNode(this->value + objValue);
}

Expression* IntegerNode::operator - (Expression* obj)
{
	log_calls("Expression* IntegerNode::operator - (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::INTEGER && objExpression->type != Expression::Type::FLOAT)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::FLOAT)
	{
		float objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode((float)this->value - objValue);
	}

	int objValue = 0;
	objExpression->evaluate(objValue);

	return new IntegerNode(this->value - objValue);
}

Expression* IntegerNode::operator * (Expression* obj)
{
	log_calls("Expression* IntegerNode::operator * (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::INTEGER && objExpression->type != Expression::Type::FLOAT)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::FLOAT)
	{
		float objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode((float)this->value * objValue);
	}

	int objValue = 0;
	objExpression->evaluate(objValue);

	return new IntegerNode(this->value * objValue);
}

Expression* IntegerNode::operator / (Expression* obj)
{
	log_calls("Expression* IntegerNode::operator / (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression); // Extract expression


	if (objExpression->type != Expression::Type::INTEGER && objExpression->type != Expression::Type::FLOAT)
	{
		std::cout << "SYNTAX ERROR: wrong types when checking equality\n";
		return nullptr;
	}

	// Int / Float edge-case
	if (objExpression->type == Expression::Type::FLOAT)
	{
		float objValue = 0;
		objExpression->evaluate(objValue);

		if (objValue == 0.0)
		{
			std::cout << "SYNTAX ERROR: division by zero\n";
			return nullptr;
		}

		return new FloatNode((float)this->value / objValue);
	}

	int objValue = 0;
	objExpression->evaluate(objValue);

	if (objValue == 0)
	{
		std::cout << "SYNTAX ERROR: division by zero\n";
		return nullptr;
	}

	return new FloatNode((float)this->value / objValue);
}

Expression* IntegerNode::operator ^ (Expression* obj)
{
	log_calls("Expression* IntegerNode::operator ^ (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::INTEGER && objExpression->type != Expression::Type::FLOAT)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::FLOAT)
	{
		float objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode(std::pow((float)this->value, objValue));
	}

	int objValue = 0;
	objExpression->evaluate(objValue);

	return new FloatNode(std::pow((float)this->value, objValue));
}

IntegerNode::~IntegerNode() {}

void IntegerNode::evaluate(int& returnValue)
{
	log_evaluations("IntegerNode::evaluate(int& returnValue)\t\t = " + std::to_string(value));
	returnValue = value;
}



FloatNode::FloatNode() {}

FloatNode::FloatNode(float value) : Expression(Expression::Type::FLOAT, false, "FloatNode", std::to_string(value))
{
	log_calls("FloatNode::FloatNode(float value)");

	this->value = value;
}

Expression* FloatNode::operator == (Expression* obj)
{
	log_calls("Expression* FloatNode::operator == (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);

	if (objExpression->type != Expression::Type::FLOAT && objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::INTEGER)
	{
		int objValue = 0;
		objExpression->evaluate(objValue);

		return new BooleanNode(this->value == (float)objValue);
	}

	float objValue = 0.0;
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

Expression* FloatNode::operator != (Expression* obj)
{
	log_calls("Expression* FloatNode::operator != (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);

	if (objExpression->type != Expression::Type::FLOAT && objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::INTEGER)
	{
		int objValue = 0;
		objExpression->evaluate(objValue);

		return new BooleanNode(this->value != (float)objValue);
	}

	float objValue = 0.0;
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value != objValue);
}

Expression* FloatNode::operator + (Expression* obj)
{
	log_calls("Expression* FloatNode::operator + (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::FLOAT && objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::INTEGER)
	{
		int objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode(this->value + (float)objValue);
	}

	float objValue = 0.0;
	objExpression->evaluate(objValue);

	return new FloatNode(this->value + objValue);
}

Expression* FloatNode::operator - (Expression* obj)
{
	log_calls("Expression* FloatNode::operator - (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::FLOAT && objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::INTEGER)
	{
		int objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode(this->value - (float)objValue);
	}

	float objValue = 0.0;
	objExpression->evaluate(objValue);

	return new FloatNode(this->value - objValue);
}

Expression* FloatNode::operator * (Expression* obj)
{
	log_calls("Expression* FloatNode::operator * (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::FLOAT && objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::INTEGER)
	{
		int objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode(this->value * (float)objValue);
	}

	float objValue = 0.0;
	objExpression->evaluate(objValue);

	return new FloatNode(this->value * objValue);
}

Expression* FloatNode::operator / (Expression* obj)
{
	log_calls("Expression* FloatNode::operator / (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::FLOAT && objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::INTEGER)
	{
		int objValue = 0;
		objExpression->evaluate(objValue);

		return new FloatNode(this->value / (float)objValue);
	}

	float objValue = 0.0;
	objExpression->evaluate(objValue);

	return new FloatNode(this->value / objValue);
}

Expression* FloatNode::operator ^ (Expression* obj)
{
	log_calls("Expression* FloatNode::operator ^ (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::FLOAT && objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	if (objExpression->type == Expression::Type::INTEGER)
	{
		int objValue = 0.0;
		objExpression->evaluate(objValue);

		return new FloatNode(std::pow(this->value, objValue));
	}

	float objValue = 0.0;
	objExpression->evaluate(objValue);

	return new FloatNode(std::pow(this->value, objValue));

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

Expression* StringNode::operator == (Expression* obj)
{
	log_calls("Expression* StringNode::operator == (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);

	if (objExpression->type != Expression::Type::STRING)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality " << this->type << ", " << objExpression->type << '\n';
		return nullptr;
	}

	std::string objValue = "";
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

Expression* StringNode::operator != (Expression* obj)
{
	log_calls("Expression* StringNode::operator != (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);

	if (objExpression->type != Expression::Type::STRING)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality " << this->type << ", " << objExpression->type << '\n';
		return nullptr;
	}

	std::string objValue = "";
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value != objValue);
}

Expression* StringNode::operator + (Expression* obj)
{
	log_calls("Expression* StringNode::operator + (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::STRING)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	std::string objValue = "";
	objExpression->evaluate(objValue);

	return new StringNode(this->value + objValue);
}

Expression* StringNode::operator * (Expression* obj)
{
	log_calls("Expression* StringNode::operator * (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression);


	if (objExpression->type != Expression::Type::INTEGER)
	{
		std::cout << "SYNTAX ERROR: wrong types when checking equality\n";
		return nullptr;
	}

	std::string result = "";
	int objValue = 0;
	objExpression->evaluate(objValue);

	for (int i = 0; i < objValue; i++)
		result += this->value;

	return new StringNode(result);
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

Expression* BooleanNode::operator == (Expression* obj)
{
	log_calls("Expression* BooleanNode::operator == (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression); // Extract the underlying expression

	if (objExpression->type != Expression::Type::BOOLEAN)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	bool objValue = false;
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value == objValue);
}

Expression* BooleanNode::operator != (Expression* obj)
{
	log_calls("Expression* BooleanNode::operator != (Expression* obj)");

	Expression* objExpression = obj;

	if (objExpression->type == Expression::Type::VARIABLE || objExpression->type == Expression::Type::PARENTHESIS)
		obj->evaluate(objExpression); // Extract the underlying expression

	if (objExpression->type != Expression::Type::BOOLEAN)
	{
		std::cout << "SYNTAX ERROR: different types when checking equality\n";
		return nullptr;
	}

	bool objValue = false;
	objExpression->evaluate(objValue);

	return new BooleanNode(this->value != objValue);
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
			this->value = "'=='";
			break;
		case BinaryOperationNode::Operation::NOT_EQUALS:
			this->value = "'!='";
			break;
		case BinaryOperationNode::Operation::PLUS:
			this->value = "'+'";
			break;
		case BinaryOperationNode::Operation::MINUS:
			this->value = "'-'";
			break;
		case BinaryOperationNode::Operation::MULTIPLICATION:
			this->value = "'*'";
			break;
		case BinaryOperationNode::Operation::DIVISION:
			this->value = "'/'";
			break;
		case BinaryOperationNode::Operation::POWER_OF:
			this->value = "'^'";
			break;
	}
}

BinaryOperationNode::~BinaryOperationNode() {}

Expression* BinaryOperationNode::execute()
{
	log_calls("Expression* BinaryOperationNode::execute()");

	if (left->isExecutable)
		left = left->execute();
	if (right->isExecutable)
		right = right->execute();

	switch(this->operation)
	{
		case BinaryOperationNode::Operation::EQUALS:
			return *left == right;
		case BinaryOperationNode::Operation::NOT_EQUALS:
			return *left != right;
		case BinaryOperationNode::Operation::PLUS:
			return *left + right;
		case BinaryOperationNode::Operation::MINUS:
			return *left - right;
		case BinaryOperationNode::Operation::MULTIPLICATION:
			return *left * right;
		case BinaryOperationNode::Operation::DIVISION:
			return *left / right;
		case BinaryOperationNode::Operation::POWER_OF:
			return *left ^ right;
	}

	return nullptr;
}



ParenthesisNode::ParenthesisNode() {}

ParenthesisNode::ParenthesisNode(Expression* expression) : Expression(Expression::Type::PARENTHESIS, true, "ParenthesisNode", "")
{
	log_calls("ParenthesisNode::ParenthesisNode(Expression* expression)");
	this->children.push_back(expression);

	this->expression = expression;
}

ParenthesisNode::~ParenthesisNode() {}

Expression* ParenthesisNode::operator == (Expression* obj)
{
	log_calls("Expression* ParenthesisNode::operator == (Expression* obj)");
	return *this->expression == obj;
}

Expression* ParenthesisNode::operator != (Expression* obj)
{
	log_calls("Expression* ParenthesisNode::operator != (Expression* obj)");
	return *this->expression != obj;
}

Expression* ParenthesisNode::operator + (Expression* obj)
{
	log_calls("Expression* ParenthesisNode::operator + (Expression* obj)");
	return *this->expression + obj;
}

Expression* ParenthesisNode::operator - (Expression* obj)
{
	log_calls("Expression* ParenthesisNode::operator - (Expression* obj)");
	return *this->expression - obj;
}

Expression* ParenthesisNode::operator * (Expression* obj)
{
	log_calls("Expression* ParenthesisNode::operator * (Expression* obj)");
	return *this->expression * obj;
}

Expression* ParenthesisNode::operator / (Expression* obj)
{
	log_calls("Expression* ParenthesisNode::operator / (Expression* obj)");
	return *this->expression / obj;
}

Expression* ParenthesisNode::operator ^ (Expression* obj)
{
	log_calls("Expression* ParenthesisNode::operator ^ (Expression* obj)");
	return *this->expression ^ obj;
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
	log_calls("Expression* ParenthesisNode::execute()");

	if (this->expression->isExecutable)
		this->expression = this->expression->execute();

	return this->expression;
}



PrintNode::PrintNode() {}

PrintNode::PrintNode(std::vector<Expression*>  expressions) : Statement("PrintNode", "")
{
	for (auto expression : expressions)
		this->children.push_back(expression);

	this->expressions = expressions;
}

PrintNode::~PrintNode() {}

Expression* PrintNode::execute()
{
	std::string output = "";

	for (auto expression : this->expressions)
	{
		if (expression->isExecutable)
			expression = expression->execute();
			
		if (expression->type == Expression::Type::VARIABLE)
			expression->evaluate(expression);

		switch (expression->type)
		{
			case Expression::Type::STRING:
			{
				expression->evaluate(output);
				break;
			}
			case Expression::Type::INTEGER:
			{
				int value = 0;
				expression->evaluate(value);
				output += std::to_string(value);
				break;
			}
			case Expression::Type::FLOAT:
			{
				float value = 0.0;
				expression->evaluate(value);
				output += std::to_string(value);
				break;
			}
			case Expression::Type::BOOLEAN:
			{
				bool value = false;
				expression->evaluate(value);
				if (value)
					output += "true";
				else
					output += "false";
				break;
			}
			case Expression::Type::PARENTHESIS:
			case Expression::Type::BINARYOPERATION:
			case Expression::Type::VARIABLE:
				break;
		}

		output += '\t';
	}

	std::cout << output << '\n';

	return nullptr;
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

	return nullptr;
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

	if (block)
	{
		this->children.push_back(block);
		this->block = block;
	}
	else
		this->block = nullptr;
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

	Expression* res = nullptr;

	for(auto statement : statements)
	{
		res = statement->execute();
	}

	return res;
}
