%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define api.token.constructor

%code {
	#define YY_DECL yy::parser::symbol_type yylex()
	YY_DECL;

	void log_grammar(std::string message)
	{
		if (debug_grammar)
			std::cout << "GRAMMAR:\t " << message << '\n';
	}

	Statement* root;
	Environment* environment = new Environment();
}

%code requires{
	#include "globals.h"
	#include "Environment.h"
	#include "Nodes.h"
}

%token EXIT 0 "end of file"

/* Control-flow */
%token <std::string> IF
%token <std::string> THEN
%token <std::string> ELSEIF
%token <std::string> ELSE

/* Looping */
%token <std::string> FOR
%token <std::string> WHILE
%token <std::string> REPEAT
%token <std::string> DO
%token <std::string> UNTIL
%token <std::string> END
%token <std::string> IN

/* Binary operations */
%token <std::string> PLUS
%token <std::string> MINUS
%token <std::string> MUL
%token <std::string> DIV
%token <std::string> POWER
%token <std::string> MOD
%token <std::string> EQUALS
%token <std::string> NOT_EQUALS
%token <std::string> LESS
%token <std::string> MORE
%token <std::string> LESS_OR_EQUAL
%token <std::string> MORE_OR_EQUAL
%token <std::string> TILDE_EQUAL

/* Unary operations */
%token <std::string> NOT
%token <std::string> AND
%token <std::string> OR
%token <std::string> HASHTAG

/* Scope */
%token <std::string> LOCAL

/* Functions */
%token <std::string> FUNCTION
%token <std::string> BREAK
%token <std::string> RETURN
%token <std::string> PRINT

/* Values */
%token <std::string> NIL
%token <std::string> FALSE
%token <std::string> TRUE
%token <int> INTEGER
%token <float> FLOAT
%token <std::string> STRING
%token <std::string> VAR

/* Single-character tokens */
%token <std::string> ASSIGNMENT
%token <std::string> DOT
%token <std::string> COLON
%token <std::string> SEMICOLON
%token <std::string> COMMA
%token <std::string> LROUND
%token <std::string> RROUND
%token <std::string> LSQUARE
%token <std::string> RSQUARE
%token <std::string> LCURLY
%token <std::string> RCURLY

/* Whitespace */
%token <std::string> WHITESPACE
%token <std::string> NEWLINE
%token <std::string> OTHER



%type <Statement*> block
%type <std::vector<Statement*>> chunk
%type <Statement*> optsemi
%type <std::vector<Statement*>> stmts
%type <Statement*> stmt
%type <Statement*> laststmt
%type <Statement*> if
%type <std::vector<Statement*>> elseifs
%type <Statement*> elseif
%type <Statement*> else

%type <Expression*> exp
%type <Expression*> var
%type <Expression*> op
%type <Expression*> op_1
%type <Expression*> op_2
%type <Expression*> op_last



%%

block : chunk								{
												log_grammar("block:chunk");
												$$ = new Block($1);
												root = $$;
											}

chunk : stmts								{
												log_grammar("chunk:stmts");
												$$ = $1;
											}
	  | chunk laststmt						{
												log_grammar("chunk:chunk laststmt");
												$$ = $1;
												if ($2)
													$$.push_back($2);
											}
	  | laststmt							{
												log_grammar("chunk:laststmt");
												if ($1)
													$$.push_back($1);
											}


laststmt : /* empty */						{
												log_grammar("laststmt:empty");
												$$ = nullptr;
											}
		 | RETURN exp/*list*/ optsemi		{
												log_grammar("laststmt:RETURN exp optsemi");
												$$ = new ReturnNode($2);
					 						}
		 | BREAK optsemi					{
												log_grammar("laststmt:BREAK optsemi");
												$$ = new BreakNode();
											}


optsemi : /* empty */						{
												log_grammar("optsemi:empty");
												$$ = nullptr;
											}
		| SEMICOLON							{
												log_grammar("optsemi:SEMICOLON");
												$$ = new SemicolonNode($1);
											}


stmts : stmt optsemi						{
												log_grammar("stmts:stmt optsemi");
												$$.push_back($1);
												if ($2)
													$$.push_back($2);
											}
	  | stmts stmt optsemi					{
												log_grammar("stmts:stmts stmt optsemi");
												$$ = $1;
												$$.push_back($2);
												if ($3)
													$$.push_back($3);
											}


stmt : if elseifs else END					{
												log_grammar("stmt:if elseifs else END");
												$2.insert($2.begin(), $1); // Insert the 'if' at front
												if ($3)
													$2.push_back($3); // Push back the 'else' if it exists
												$$ = new IfStatementNode($2);
											}
	 | var ASSIGNMENT exp					{
												log_grammar("stmt:var ASSIGNMENT exp");
												$$ = new AssignmentNode(environment, $1, $3);
											}
	 | PRINT exp							{
												log_grammar("stmt:PRINT exp");
												$$ = new PrintNode($2);
											}


if : IF exp THEN block						{
												log_grammar("if:IF exp THEN block");
												$$ = new IfNode($2, $4);
											}


elseifs : /* empty */						{
												log_grammar("elseifs:empty");
											}
		| elseif							{
												log_grammar("elseifs: ELSEIF");
												$$.push_back($1);
											}
		| elseifs elseif					{
												log_grammar("elseifs:elseifs elseif");
												$$ = $1;
												$$.push_back($2);
											}


elseif : ELSEIF exp THEN block				{
												log_grammar("elseif:ELSEIF exp THEN block");
												$$ = new IfNode($2, $4);
											}


else : /* empty */							{
												log_grammar("else:empty");
												//$$ = new ElseNode();
											}
	 | ELSE block							{
												log_grammar("else:ELSE block");
												$$ = new ElseNode($2);
											}


exp : op									{
												log_grammar("exp:op");
												$$ = $1;
											}


op : op_1									{
												log_grammar("op:op_1");
												$$ = $1;
											}
   | op EQUALS op_1							{
												log_grammar("op:op EQUALS op_1");
												$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::EQUALS);
											}
   | op NOT_EQUALS op_1						{
												log_grammar("op:op NOT_EQUALS op_1");
												$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::NOT_EQUALS);
											}


op_1 : op_2									{
												log_grammar("op_1:op_2");
												$$ = $1;
											}
	 | op_1 PLUS op_2						{
												log_grammar("op_1:op_1 PLUS op_2");
												//$$ = new PlusNode($1, $3);
												$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::PLUS);
											}
	 | op_1 MINUS op_2						{
												log_grammar("op_1:op_1 MINUS op_2");
												//$$ = new PlusNode($1, $3);
												$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::MINUS);
											}

op_2 : op_last								{
												log_grammar("op_2:op_last");
												$$ = $1;
											}
	 | op_2 MUL op_last						{
 												log_grammar("op_2:op_2 MUL op_last");
 												$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::MULTIPLICATION);
 											}
	 | op_2 DIV op_last						{
												log_grammar("op_2:op_2 DIV op_last");
												$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::DIVISION);
											}


op_last : TRUE								{
												log_grammar("op_last:TRUE");
												$$ = new BooleanNode(true);
											}
		| FALSE								{
												log_grammar("op_last:FALSE");
												$$ = new BooleanNode(false);
											}
		| INTEGER							{
												log_grammar("op_last:INTEGER");
												$$ = new IntegerNode($1);
											}
		| FLOAT								{
 												log_grammar("op_last:FLOAT");
 												$$ = new FloatNode($1);
 											}
		| STRING							{
												log_grammar("op_last:STRING");
												$$ = new StringNode($1);
											}
		| VAR								{
												log_grammar("op_last:VAR");
												$$ = new VariableNode(environment, $1);
											}
		| LROUND exp RROUND					{
												log_grammar("op_last:LROUND exp RROUND");
												$$ = new ParenthesisNode($2);
											}


var : VAR									{
												log_grammar("var:VAR");
												$$ = new VariableNode(environment, $1);
											}
