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
%token <std::string> POWER_OF
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
%token <std::string> IO_WRITE
%token <std::string> IO_READ
%token <std::string> IO_READ_NUMBER
%token <std::string> IO_READ_LINE
%token <std::string> IO_READ_ALL

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
%type <std::vector<Statement*>> stmts
%type <Statement*> stmt
%type <Statement*> laststmt
%type <Statement*> repeatuntil
%type <Statement*> for
%type <Statement*> assignment
%type <Statement*> if
%type <std::vector<Statement*>> elseifs
%type <Statement*> elseif
%type <Statement*> else

%type <Expression*> ioread
%type <std::vector<Expression*>> explist
%type <Expression*> exp
%type <Expression*> op
%type <Expression*> op_1
%type <Expression*> op_2
%type <Expression*> op_3
%type <Expression*> op_last



%%

block : chunk								{ log_grammar("block:chunk"); $$ = new Block($1); root = $$; }

chunk : stmts								{ log_grammar("chunk:stmts"); 						$$ = $1; }
	  | chunk laststmt						{ log_grammar("chunk:chunk laststmt"); 				$$ = $1; $$.push_back($2); }
	  | chunk laststmt SEMICOLON			{ log_grammar("chunk:chunk laststmt SEMICOLON"); 	$$ = $1; $$.push_back($2); }

laststmt : RETURN exp/*list*/ 				{ log_grammar("laststmt:RETURN exp optsemi"); 	$$ = new ReturnNode($2); }
		 | BREAK 							{ log_grammar("laststmt:BREAK optsemi"); 		$$ = new BreakNode(); 	}

stmts : stmt								{ log_grammar("stmts:stmt"); 				$$.push_back($1); }
	  | stmt SEMICOLON						{ log_grammar("stmts:stmt"); 				$$.push_back($1); }
	  | stmts stmt							{ log_grammar("stmts:stmts stmt optsemi"); 	$$ = $1; $$.push_back($2); }
	  | stmts SEMICOLON stmt				{ log_grammar("stmts:stmts stmt optsemi"); 	$$ = $1; $$.push_back($3); }

stmt : if elseifs else END					{ log_grammar("stmt:ifstatement END");					$2.insert($2.begin(), $1); if ($3) $2.push_back($3); $$ = new IfStatementNode($2); }
	 | assignment							{ log_grammar("stmt:assignment");						$$ = $1; }
	 | PRINT explist						{ log_grammar("stmt:PRINT explist"); 					$$ = new PrintNode($2); }
	 | PRINT LROUND explist RROUND			{ log_grammar("stmt:PRINT LROUND explist RROUND");		$$ = new PrintNode($3); }
	 | for END								{ log_grammar("stmt:for END"); 							$$ = $1; }
	 | IO_WRITE LROUND explist RROUND		{ log_grammar("stmt:IO_WRITE LROUND explist RROUND");	$$ = new IOWriteNode($3); }
	 | repeatuntil							{ log_grammar("stmt:repeatuntil"); 						$$ = $1; }

repeatuntil : REPEAT block UNTIL exp		{ log_grammar("repeatuntil:REPEAT block UNTIL exp"); $$ = new RepeatStatementNode($2, $4); }

ioread : IO_READ LROUND exp RROUND				{ log_grammar("ioread:IO_READ (exp)");		$$ = new IOReadNode($3); }
 	   | IO_READ LROUND IO_READ_NUMBER RROUND	{ log_grammar("ioread:IO_READ (*number)");	$$ = new IOReadNode($3); }
 	   | IO_READ LROUND IO_READ_LINE RROUND		{ log_grammar("ioread:IO_READ (*line)");	$$ = new IOReadNode($3); }
	   | IO_READ LROUND IO_READ_ALL RROUND		{ log_grammar("ioread:IO_READ (*all)");		$$ = new IOReadNode($3); }

assignment : VAR ASSIGNMENT exp						{ log_grammar("assignment:VAR = exp");			$$ = new AssignmentNode(environment, new VariableNode(environment, $1), $3); }
 		   | VAR ASSIGNMENT ioread					{ log_grammar("assignment:VAR = ioread");		$$ = new AssignmentNode(environment, new VariableNode(environment, $1), $3); }
		   | VAR ASSIGNMENT LCURLY explist RCURLY	{ log_grammar("assignment:VAR = { explist }");	$$ = new AssignmentNode(environment, new VariableNode(environment, $1), new ListNode($4)); }

for : FOR VAR ASSIGNMENT explist DO block	{ log_grammar("for:FOR VAR ASSIGNMENT exp COMMA exp"); $$ = new ForNode(new VariableNode(environment, $1), $4, $6); }

if : IF exp THEN block						{ log_grammar("if:IF exp THEN block"); $$ = new IfNode($2, $4); }

elseifs : /* empty */						{ log_grammar("elseifs:empty"); }
		| elseif							{ log_grammar("elseifs: ELSEIF");			$$.push_back($1); 	}
		| elseifs elseif					{ log_grammar("elseifs:elseifs elseif");	$$ = $1; $$.push_back($2); }

elseif : ELSEIF exp THEN block				{ log_grammar("elseif:ELSEIF exp THEN block"); $$ = new IfNode($2, $4); }

else : /* empty */							{ log_grammar("else:empty"); }
	 | ELSE block							{ log_grammar("else:ELSE block"); $$ = new ElseNode($2); }
//
explist : exp 								{ log_grammar("explist:exp");			$$.push_back($1); }
		| explist COMMA exp					{ log_grammar("explist:explist exp");	$$ = $1; $$.push_back($3); }

exp : op									{ log_grammar("exp:op"); $$ = $1; }

op : op_1									{ log_grammar("op:op_1");		$$ = $1; }
   | op EQUALS op_1							{ log_grammar("op:op == op_1");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::EQUALS); }
   | op NOT_EQUALS op_1						{ log_grammar("op:op != op_1");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::NOT_EQUALS); }

op_1 : op_2									{ log_grammar("op_1:op_2");			$$ = $1; }
	 | op_1 PLUS op_2						{ log_grammar("op_1:op_1 + op_2");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::PLUS); }
	 | op_1 MINUS op_2						{ log_grammar("op_1:op_1 - op_2");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::MINUS); }

op_2 : op_3									{ log_grammar("op_2:op_3"); 		$$ = $1; }
	 | op_2 MUL op_3						{ log_grammar("op_2:op_2 * op_3");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::MULTIPLICATION); }
	 | op_2 DIV op_3						{ log_grammar("op_2:op_2 / op_3");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::DIVISION); }
	 | op_2 MOD op_3						{ log_grammar("op_2:op_2 % op_3");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::MODULUS); }

op_3 : op_last								{ log_grammar("op_3:op_last");			$$ = $1; }
	 | op_3 POWER_OF op_last				{ log_grammar("op_3:op_3 ^ op_last");	$$ = new BinaryOperationNode($1, $3, BinaryOperationNode::Operation::POWER_OF); }

op_last : TRUE								{ log_grammar("op_last:TRUE"); 				$$ = new BooleanNode(true); }
		| FALSE								{ log_grammar("op_last:FALSE"); 			$$ = new BooleanNode(false); }
		| FLOAT								{ log_grammar("op_last:FLOAT"); 			$$ = new FloatNode($1); }
		| INTEGER							{ log_grammar("op_last:INTEGER"); 			$$ = new IntegerNode($1); }
		| STRING							{ log_grammar("op_last:STRING"); 			$$ = new StringNode($1); }
		| VAR								{ log_grammar("op_last:VAR"); 				$$ = new VariableNode(environment, $1); }
		| VAR LSQUARE exp RSQUARE			{ log_grammar("op_last:[exp]"); 			$$ = new VariableNode(environment, $1, $3); }
		| LROUND exp RROUND					{ log_grammar("op_last:(exp)"); 			$$ = new ParenthesisNode($2); }
