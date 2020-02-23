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
			std::cout << "GRAMMAR : " << message << '\n';
	}

	Node* root;
}

%code requires{
	#include "globals.h"



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
%token <std::string> STATIC

/* Values */
%token <std::string> NIL
%token <std::string> FALSE
%token <std::string> TRUE
%token <std::string> INTEGER
%token <std::string> FLOAT
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



%type <Node*> block
%type <Node*> chunk
%type <Node*> stmts
%type <Node*> stmt
%type <Node*> laststmt
%type <Node*> optsemi
%type <Node*> if
%type <Node*> elseifs
%type <Node*> elseif
%type <Node*> else
%type <Node*> exp



%%

block : chunk								{
												log_grammar("block:chunk");
												$$ = $1;
												root = $$;
											}

chunk : stmts								{
												log_grammar("chunk:stmts");
												$$ = $1;
											}
	  | chunk laststmt						{
												log_grammar("chunk:chunk laststmt");
												$$ = $1;
												$$->children.push_back($2);
											}
	  | laststmt							{
												log_grammar("chunk:laststmt");
												$$ = new Node("block", "");
												$$->children.push_back($1);
											}


stmts : stmt optsemi						{
												log_grammar("stmts:stmt optsemi");
												$$ = new Node("block", "");
												$$->children.push_back($1);
											}
	  | stmts stmt optsemi					{
												log_grammar("stmts:stmts stmt optsemi");
												$$ = $1;
												$$->children.push_back($2);
											}


stmt : if elseifs else END					{
												log_grammar("stmt:if elseifs else END");
												$$ = new Node("if statement", "");
												$$->children.push_back($1);
												$$->children.push_back($2);
												$$->children.push_back($3);
											}
	 | VAR ASSIGNMENT VAR					{
												log_grammar("stmt:VAR ASSIGNMENT VAR");
												$$ = new Node("var assignment", "");
												$$->children.push_back(new Node("var", $1));
												$$->children.push_back(new Node("var", $3));
											}


laststmt : /* empty */						{
												log_grammar("laststmt:empty");
												$$ = new Node("last statement", "");
											}
		 | RETURN exp optsemi/*list*/		{
												log_grammar("laststmt:RETURN exp optsemi");
												$$ = new Node("last statement", $1);
												$$->children.push_back($2);
					 						}
		 | BREAK optsemi					{
												 log_grammar("laststmt:BREAK optsemi");
												 $$ = new Node("last statement", $1);
											}


optsemi : /* empty */						{
												log_grammar("optsemi:empty");
												$$ = new Node("optsemi", "");
											}
		| SEMICOLON							{
												log_grammar("optsemi:SEMICOLON");
												$$ = new Node("optsemi", $1);
											}


if : IF exp THEN block						{
												log_grammar("if:IF exp THEN block");
												$$ = new Node("if", "");
												$$->children.push_back($2);
												$$->children.push_back($4);
											}


elseifs : /* empty */						{
												log_grammar("elseifs:empty");
												$$ = new Node("elseifs", "");
											}
		| elseif							{
												log_grammar("elseifs: ELSEIF");
												$$ = new Node("elseifs", "");
												$$->children.push_back($1);
											}
		| elseifs elseif					{
												log_grammar("elseifs:elseifs elseif");
												$$ = $1;
												$$->children.push_back($2);
											}


elseif : ELSEIF exp THEN block				{
												log_grammar("elseif:ELSEIF exp THEN block");
												$$ = new Node("elseif", "");
												$$->children.push_back($2);
												$$->children.push_back($4);
											}


else : /* empty */							{
												log_grammar("else:empty");
												$$ = new Node("else", "");
											}
	 | ELSE block							{
												log_grammar("else:ELSE block");
												$$ = new Node("else", "");
												$$->children.push_back($2);
											}


exp : TRUE									{
												log_grammar("exp:TRUE");
												$$ = new Node("exp", $1);
											}
	| FALSE									{
												log_grammar("exp:FALSE");
												$$ = new Node("exp", $1);
											}




/*
stream : optline				{
									std::cout << "stream:optline\n";
									$$ = new Node("stream", "");
									$$->children.push_back($1);
									root = $$;
 								}
	   | stream NEWLINE optline {
								   std::cout << "stream:stream optline\n";
								   $$ = $1;
								   $$->children.push_back($3);
								   root = $$;
								}


optline : /*empty*/	/*			{
									std::cout << "-optline:\n";
									$$ = new Node("optline", "empty");
								}
		| line					{
									std::cout << "-optline:line\n";
									$$ = $1;
								}
		;


line : stmt						{
									std::cout << "line:stmt\n";
									$$ = new Node("stmt", "");
									$$->children.push_back($1);
								}


stmt : expr						{
									std::cout << "line:expr\n";
									$$ = new Node("Expression", "");
									$$->children.push_back($1);
									//$$->children.push_back($3);
								}
	 | VAR						{ $$ = new Node("VAR", $1); }

expr : term						{
									std::cout << "expr:term\n";
									$$ = $1;
								}
	 | expr PLUS term			{
									std::cout << "expr:expr + term\n";
									$$ = new Node("Addition", "");
									$$->children.push_back($1);
									$$->children.push_back($3);
								}
	 | expr MINUS term			{
									std::cout << "expr:expr - term\n";
									$$ = new Node("Subtraction", "");
									$$->children.push_back($1);
									$$->children.push_back($3);
								}

term : fact						{
									std::cout << "term:fact\n";
									$$ = $1;
								}
	 | term MUL fact			{
									std::cout << "term:term * fact\n";
									$$ = new Node("Multiplication", "");
									$$->children.push_back($1);
									$$->children.push_back($3);
								}
	 | term DIV fact			{
									std::cout << "term:term * fact\n";
									$$ = new Node("Division", "");
									$$->children.push_back($1);
									$$->children.push_back($3);
								}


fact : number					{
									std::cout << "fact:INTEGER\n";
									$$ = $1;
								}
	 | LROUND expr RROUND				{
									std::cout << "fact:(expr)\n";
									$$ = new Node("Paranthesis", "");
									$$->children.push_back($2);
								}
	 | MINUS fact				{
								   $$ = new Node("Minus", "");
								   $$->children.push_back($2);
							   }



number : INTEGER 				{ $$ = new Node("INTEGER", $1); }
	   | FLOAT 					{ $$ = new Node("FLOAT", $1); }
*/

/*
anything : FUNC 		 		{ $$ = new Node("FUNC", $1); }
		 | STATIC		 		{ $$ = new Node("STATIC", $1); }
		 | VAR 	 				{ $$ = new Node("VAR", $1); }
		 | NEWLINE 	 			{ $$ = new Node("NEWLINE", $1); }
		 | BLANK 				{ $$ = new Node("BLANK", $1); }
		 | QUOTED 				{ $$ = new Node("QUOTED", $1); }
		 | DBLQUOTED 			{ $$ = new Node("DBLQUOTED", $1); }
		 | BINOP 				{ $$ = new Node("BINOP", $1); }
		 | LROUND 				{ $$ = new Node("LROUND", $1); }
		 | RROUND 				{ $$ = new Node("RROUND", $1); }
		 | LSQUARE 				{ $$ = new Node("LSQUARE", $1); }
		 | RSQUARE 				{ $$ = new Node("RSQUARE", $1); }
		 | LCURLY 				{ $$ = new Node("LCURLY", $1); }
		 | RCURLY 				{ $$ = new Node("RCURLY", $1); }
		 | LEN 					{ $$ = new Node("LEN", $1); }
		 | INTEGER 				{ $$ = new Node("INTEGER", $1); }
		 | FLOAT 				{ $$ = new Node("FLOAT", $1); }
		 | OTHER 				{ $$ = new Node("OTHER", $1); }
 		 ;
*/
