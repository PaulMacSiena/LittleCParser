/*
  Adapted from front.c by Dr. Jim Teresco, Professor at Siena College
  
  Adaptations for Little C made by Paul Macfarlane and Cheryl McClean Fall 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* define the longest allowable lexeme length */
#define MAX_LEXEME_LENGTH 100

/* global variables used to avoid passing around lots of parameters -
   think of them like the instance variables of a class */

int charClass;
char lexeme[MAX_LEXEME_LENGTH];
char nextChar;
int lexLen;
int nextToken;
FILE *in_fp;
int level;

/* Function prototypes */
void addChar();
void getChar();
void getNonBlank();
int lex();
void expr();
void term();
void factor();
void error(char *);
void entryMsg(char *);
void exitMsg(char *);
void match(char *);
void program();
void compound_statement();
void conditional_statement();
void declaration_list();
void declaration();
void statement_list();
void statement();
void expression_statement();
void iterative_statement();
void while_statement();
void for_statement();
void for_expressions(); 
void assignment_expression();
void type_specifier();
void floating_type_specifier();
void integer_type_specifier();
void initialized_declarator_list();
void conditional_expression();
void logical_or_expression();
void logical_and_expression();
void equality_expression();
void relational_expression();
void additive_expression();
void multiplicative_expression();
void primary_expression();
void parenthesized_expression();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define FLOAT_LIT 12
#define NEG_INT_LIT 13
#define NEG_FLOAT_LIT 14
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define EQUAL_OP 25
#define LESSTHAN_OP 26
#define GREATERTHAN_OP 27
#define LTEQ_OP 28
#define GTEQ_OP 29
#define NOTEQ_OP 30
#define AND_OP 31
#define OR_OP 32 
#define MOD_OP 33 
#define LEFT_PAREN 40
#define RIGHT_PAREN 41
#define SCOLON_TERM 42 
#define LEFT_BRACKET 43 
#define RIGHT_BRACKET 44 
#define OPEN_BRACKET 45
#define CLOSE_BRACKET 46
#define COLON 47
#define IF_KEY 50 
#define ELSE_KEY 51
#define FOR_KEY 52
#define FLOAT_KEY 53
#define INT_KEY 54
#define COMMA 60
#define QUOTE 61
#define PERIOD 62
#define WHILE_KEY 63
#define VOID_KEY 90
#define MAIN_KEY 91 

/* main driver -- now performing recursive descent parsing */
int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s filename\n", argv[0]);
    exit(1);
  }

  /* open the input data file and process contents */
  if ((in_fp = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "%s: cannot open input file %s for reading\n", 
	    argv[0], argv[1]);
    exit(1);
  }

  /* indicate we have not yet entered a production */
  level = 0;

  /* we have our input, we now proceed with the parsing of an expression */
  getChar();  // initialize our lexer
  entryMsg("program");
  lex();
  program();

  fclose(in_fp);
  lex();
  if (nextToken != EOF) {
    error("Expected EOF.");
  }
  else {
    printf("Little C Parsing Completed!!!!\n");
  }
  return 0;
}

/* function to return a printable representation of the defined
   token codes above, given the numeric code */
/* for output purposes only, not part of the lexer */
char *tokenCode(int code) {

  switch (code) {

  case INT_LIT:
    return "INT_LIT";

  case IDENT:
    return "IDENT";

  case FLOAT_LIT:
    return "FLOAT_LIT";
  
  case NEG_INT_LIT:
    return "NEG_INT_LIT";

  case NEG_FLOAT_LIT:
    return "NEG_FLOAT_LIT";

  case ASSIGN_OP:
    return "ASSIGN_OP";

  case ADD_OP:
    return "ADD_OP";

  case SUB_OP:
    return "SUB_OP";

  case MULT_OP:
    return "MULT_OP";

  case DIV_OP:
    return "DIV_OP";

  case EQUAL_OP:
    return "EQUAL_OP";

  case LESSTHAN_OP:
    return "LESSTHAN_OP";

  case GREATERTHAN_OP:
    return "GREATERTHAN_OP";

  case LTEQ_OP:
    return "LTEQ_OP";

  case GTEQ_OP:
    return "GTEQ_OP";
  
  case NOTEQ_OP:
    return "NOTEQ_OP";
 
  case AND_OP:
    return "AND_OP";
   
  case OR_OP:
    return "OR_OP";
  
  case MOD_OP:
    return "MOD_OP";

  case LEFT_PAREN:
    return "LEFT_PAREN";

  case RIGHT_PAREN:
    return "RIGHT_PAREN";
    
  case SCOLON_TERM:
    return "SCOLON_TERM";
  
  case LEFT_BRACKET:
    return "LEFT_BRACKET";

  case RIGHT_BRACKET:
    return "RIGHT_BRACKET";

  case IF_KEY:
    return "IF_KEY";
    
  case ELSE_KEY:
    return "ELSE_KEY";

  case FOR_KEY:
    return "FOR_KEY";

  case WHILE_KEY:
    return "WHILE_KEY";
 
  case VOID_KEY:
    return "VOID_KEY";

  case MAIN_KEY:
    return "MAIN_KEY";

  case FLOAT_KEY:
    return "FLOAT_KEY";
  
  case INT_KEY:
    return "INT_KEY";

  case COMMA:
    return "COMMA";

  case OPEN_BRACKET:
    return "OPEN_BRACKET";
 
  case CLOSE_BRACKET:
    return "CLOSE_BRACKET";

  case QUOTE:
    return "QOUTE";

  case COLON:
    return "COLON";

  case PERIOD:
    return "PERIOD";

  default:
    return "UNKNOWN";
  }
}

/*
  keyWordLookup - a function to look up potential keywords
 */
int keywordLookup(char* word){
  if (strcmp(word, "if") == 0){
    nextToken = IF_KEY;
  }
  else if (strcmp(word, "else") == 0){
    nextToken = ELSE_KEY;
  }
  else if (strcmp(word, "for") == 0){
    nextToken = FOR_KEY;
  }
  else if (strcmp(word, "while") == 0){
    nextToken = WHILE_KEY;
  }
  else if (strcmp(word, "main") == 0){
    nextToken = MAIN_KEY;
  }
  else if (strcmp(word, "void") == 0){
    nextToken = VOID_KEY;
  }
  else if (strcmp(word, "float") == 0){
    nextToken = FLOAT_KEY;
  }
  else if (strcmp(word, "int") == 0){
    nextToken = INT_KEY;
  }
  else {
    nextToken = UNKNOWN;
  }
  return nextToken;
}

/* lookup - a function to look up operators and parentheses -
   note that this both returns one of the token constants defined
   above and sets the nextToken global variable.
 */
int lookup(char ch) {

  switch (ch) {
  case '>':
    addChar();
    getChar();
    if (nextChar == '='){
      nextToken = GTEQ_OP;
      addChar();
    }
    else{
      nextToken = GREATERTHAN_OP;
    }
    break;
  case '<':
    addChar();
    getChar();
    nextToken = LESSTHAN_OP;
    if (nextChar == '='){
      nextToken= LTEQ_OP;
      addChar();
    }
    else{
      nextToken = LESSTHAN_OP;
    }
    break;
  case '(':
    addChar();
    nextToken = LEFT_PAREN;
    break;
  case ')':
    addChar();
    nextToken = RIGHT_PAREN;
    break;
  case '+':
    addChar();
    nextToken = ADD_OP;
    break;
  case '-':
    addChar();
    nextToken = SUB_OP;
    break;
  case '*':
    addChar();
    nextToken = MULT_OP;
    break;
  case '/':
    addChar();
    nextToken = DIV_OP;
    break;
  case '=':
    addChar();
    getChar();
    if (nextChar=='='){
      nextToken = EQUAL_OP;
      addChar();
    }
    else {
      nextToken = ASSIGN_OP;
    }
    break;
  case '!':
    addChar();
    getChar();
    if (nextChar=='='){
      nextToken = NOTEQ_OP;
      addChar();
    }
    else {
      nextToken = UNKNOWN;
    }
    break;
  case '&':
    addChar();
    getChar();
    if (nextChar == '&'){
      nextToken = AND_OP;
      addChar();
    }
    else {
      nextToken= UNKNOWN;
    }      
    break;
  case '|':
    addChar();
    getChar();
    if (nextChar == '|'){
      nextToken = OR_OP;
      addChar();
    }
    else {
      nextToken= UNKNOWN;                                                                                                                                                         
    }
    break;
  case '%':
    addChar();
    nextToken = MOD_OP;
    break;
  case ';':
    addChar();
    nextToken = SCOLON_TERM;
    break;
  case '[':
    addChar();
    nextToken = LEFT_BRACKET;
    break;
  case ']':
    addChar();
    nextToken = RIGHT_BRACKET;
    break;
  case ',':
    addChar();
    nextToken = COMMA;
    break;
  case '{':
    addChar();
    nextToken = OPEN_BRACKET;
    break;
  case '}':
    addChar();
    nextToken = CLOSE_BRACKET;
    break;
  case '"':
    addChar();
    nextToken = QUOTE;
    break;
  case ':':
    addChar();
    nextToken = COLON;
    break;
  case '.':
    addChar();
    nextToken = PERIOD;
    break;
  default:
    addChar();
    nextToken = EOF;
    break;
  }
  return nextToken;
}

/* addChar - a function to add next char to lexeme */
void addChar(){

  if (lexLen <= (MAX_LEXEME_LENGTH-2)) {
    lexeme[lexLen++] = nextChar;
    lexeme[lexLen] = '\0';
  } 
  else {
    fprintf(stderr, "Error - lexeme too long: %s \n", lexeme);
  }
}

/* getChar - a function get the next char of input and determine
   its character class 

   sets global variables nextChar and charClass.
*/
void getChar(){

  if ((nextChar = getc(in_fp)) != EOF) {
    if (isalpha(nextChar))
      charClass = LETTER;
    else if (isdigit(nextChar))
      charClass = DIGIT;
    else 
      charClass = UNKNOWN;
  } 
  else {
    charClass = EOF;
  }
}

/* getNonBlank - a function to call getChar until it
   returns a non-whitespace character */
void getNonBlank(){

  while (isspace(nextChar))
    getChar();
}

/* lex - a simple lexical analyzer for arithmetic expressions.

   One call parses one lexeme.  And here, prints out some info about it.
 */
int lex(){

  lexLen = 0;
  getNonBlank();
  switch (charClass) {
  case LETTER: //take care of keywords
    /* parse identifiers (would also match keywords if we had any) */
    /* identifiers here start with a letter then can be followed by
       any number of letters and numbers */
    addChar();  // build lexeme
    getChar();  // move to next char of input, see if still part of an IDENT
    while (charClass == LETTER || charClass == DIGIT){
      addChar(); // continue to build lexeme
      getChar(); // and on to next char
    }
    keywordLookup(lexeme);
    if (nextToken == 99){ //if != 99, if is a keyword
      nextToken = IDENT;
    }
    break;
    
  case DIGIT:
    /* parse integer literals - starts with a digit, continue until we
       encounter something else */
    addChar();
    getChar();
    while (charClass == DIGIT ){
      addChar();
      getChar();
    }
    if (nextChar == '.'){
      nextToken = FLOAT_LIT;
      addChar();
      getChar();
      while (charClass == DIGIT){ 
	addChar();
	getChar();
      }
    }
    else{
      nextToken = INT_LIT;
    }
    break;

  case UNKNOWN:
    /* Parentheses and operators -- all are single character in our
       simple language.   So they become tokens in their own right, if
       recognized by lookup, become EOF otherwise.
    */
    
    //take char of negative int LITS
    if (nextChar == '-'){
      addChar();
      getChar();
      while (charClass == DIGIT ){
	addChar();
	getChar();
      }
      if (nextChar == '.'){
	nextToken = NEG_FLOAT_LIT;
	addChar();
	getChar();
	while (charClass == DIGIT){
	  addChar();
	  getChar();
	}
      }
      else{
	nextToken = NEG_INT_LIT;
      }
      break;
    }
    if (nextChar =='.'){
      addChar();
      getChar();
      while (charClass == DIGIT){ 
        addChar();
        getChar();
	nextToken = FLOAT_LIT;
      }
      break;
    }
    lookup(nextChar);
    getChar();
    break;


  case EOF:
    /* EOF -- return a special EOF token */
    nextToken = EOF;
    lexeme[0] = 'E';
    lexeme[1] = 'O';
    lexeme[2] = 'F';
    lexeme[3] = '\0';
    break;

  } /* end of switch */

  printf("Next token is: %2d (%12s), next lexeme is %s\n",
	 nextToken, tokenCode(nextToken), lexeme);
  return nextToken;
} /* end lex */

/* recursive descent production rule functions here */

/* Function program 
   Parses string in the language
   generated by the rule:
   <program> => void main () <compound-statement>
 */
void program(){
  
  entryMsg("program");
  if (nextToken != VOID_KEY){
    error("Could not parse <program>"); /* No void found */
  }
  match("program");
  lex();
  if (nextToken != MAIN_KEY){
    error("Could not parse <program>"); /* No main found */
  }
  match("program");
  lex();
  if (nextToken!= LEFT_PAREN){
    error("Could not parse <program>"); /* No ( found */
  }
  match("program");
  lex();
  if (nextToken!= RIGHT_PAREN){
    error("Could not parse <program>"); /* No ) found */
  }
  match("program");
  
  lex();
  compound_statement();
  
  exitMsg("program");
}

/*Function compound_statement
  Parses string in the language generated by the rule
  <compound-statement> ::= { [<declaration-list>] [<statement-list>] }
 */
void compound_statement(){
  entryMsg("compound_statement");
  
  if (nextToken!=OPEN_BRACKET){
    error("Could not parse <compound_statement>"); /* No { found */
  }
  match("compound_statement");
  
  lex();
  //if the nextToken is 'float' or 'int', we could have a declaration list
  if (nextToken == FLOAT_KEY || nextToken == INT_KEY){
    match("compound_statement");
    declaration_list();
  }
  //declaration list calls lex at the end, so next token is already scanned
  
  // if any of these tokens are the next token we have to see if it is a statment list
  if (nextToken == IDENT || nextToken == OPEN_BRACKET || nextToken == IF_KEY
      || nextToken == WHILE_KEY || nextToken == FOR_KEY || nextToken == SCOLON_TERM){
    statement_list();
  }

  //lex called at end of statement_list()
  if (nextToken!= CLOSE_BRACKET){
    error("Could not parse <compound_statement>");
  }
  match("compound_statement");

  exitMsg("compound_statement");
}

/*Function declaration_list
  Parses strings in the language generated by the rule
  <declaration-list> ::= <declaration> [ <declaration-list> ]
*/
void declaration_list(){
  entryMsg("declaration_list");

  //if we got this far, we know we at least have the beginning of a declaration
  // 'float' or 'int'
  declaration();
  
  //take care of optional declaration list, if nextToken is 'int' or 'float' key, then we have to check
  lex();
  if (nextToken == FLOAT_KEY || nextToken == INT_KEY){
    match("declaration_list");
    declaration_list();
  }
  
  exitMsg("declaration_list");
}

/*Function declaration
  Parses strings in the language generated by the rule:
  <declaration> ::= <type-specifier> <initialized-declarator-list> ;
*/
void declaration(){
  entryMsg("declaration");

  //if we got this far, we know we have a type specifier
  match("declaration");
  type_specifier();

  //if we got this far, we have to find out if we have an IDL
  initialized_declarator_list();

  //lex called at end of initialized_declarator_list(), so we already have nextToken
  if (nextToken != SCOLON_TERM){
    error("could not parse <declaration>");
  }
  match("declaration");

  exitMsg("declaration");
}

/*Function initialized_declarator_list
  Parses strings in the language generated by the rule:
  <initialized-declarator-list> ::= IDENT [, <initialized-declarator-list> ]
 */
void initialized_declarator_list(){
  entryMsg("initialized_declarator_list");

  lex();
  if (nextToken != IDENT){
    error("could not parse <initialized_declarator_list>");
  }
  match("initialized_declarator_list");

  lex();
  if (nextToken == COMMA){
    match("initialized_delcarator_list");
    initialized_declarator_list();
  }

  exitMsg("initialized_declarator_list");
}

/*Function type_specifier
  Parses strings in the language generated by the rule:
  <type-specifier> ::= <floating-type-specifier> | <integer-type-specifier>
*/
void type_specifier(){
  entryMsg("type_specifier");

  //lex was already called, we already have nextToken
  if (nextToken == FLOAT_KEY){
    floating_type_specifier();
  }
  else if (nextToken == INT_KEY){
    integer_type_specifier();
  }
  else{
    error("could not parse <type_specifier>");
  }

  exitMsg("type_specifier");
}

/*Function floating_type_specifier
  Parses strings in the language generated by the rule
  <floating-type-specifier> ::= float
*/
void floating_type_specifier(){
  entryMsg("floating_type_specifier");

  if (nextToken == FLOAT_KEY){ 
    match("floating_type_specifier");
  }
  exitMsg("floating_type_specifier");
}

/*Function integer_type_specifier
  Parses strings in the language generated by the rule:
  <integer-type-specifier> ::= int
 */
void integer_type_specifier(){
  entryMsg("integer_type_specifier");
  
  if (nextToken == INT_KEY){
    match("integer_type_specifier");
  }
  
  exitMsg("integer_type_specifier");
}

/*Function statement_list
  Parses strings in the language generated by the rule:
  <statement-list> ::= <statement> [ <statement-list> ]
 */
void statement_list(){

  entryMsg("statement_list");

  //if we got this far, we know we are checking for a statment
  statement();
  
  lex(); 
  //optional statement List, if nextoken is one of these, we have to check for another statement list
  if (nextToken == IDENT || nextToken == OPEN_BRACKET || nextToken == IF_KEY
      || nextToken == WHILE_KEY || nextToken == FOR_KEY || nextToken == SCOLON_TERM){
    statement_list(); 
  }

  exitMsg("statement_list");
}

/*Function statement
  Parses strings in the language generated by the rule
  <statement> ::= <expression-statement> | <compound-statement>
           | <conditional-statement> | <iterative-statement>
           | <null-statement>
 */
void statement(){
  entryMsg("statement");

  if (nextToken == IDENT){
    expression_statement(); 
  }
  else if (nextToken == OPEN_BRACKET){
    compound_statement();
  }
  else if (nextToken == IF_KEY){
    conditional_statement();
  }
  else if (nextToken == WHILE_KEY || nextToken == FOR_KEY){
    iterative_statement();
  }
  else if (nextToken == SCOLON_TERM){
    entryMsg("null_statement");
    match("null_statement");
    exitMsg("null_statement");
  }
  else{
    error("could not parse <statement>"); 
  }

  exitMsg("statement");
}

/*Function expression_statement
  Parses strings in the language generated by the rule
  <expression-statement> ::= <assignment-expression> ;
 */
void expression_statement(){
  entryMsg("expression_statement");

  assignment_expression();

  if (nextToken == ELSE_KEY){
    lex();
    statement();
    exitMsg("expression_statement");
  }
  else if (nextToken != SCOLON_TERM){
    error("could not parse <expression_statement>");
  }
  else {
    match("expression_statement");
    exitMsg("expression_statement");
  }
}

/*Function assignment_expression
  Parses strings in the language generated by the rule
  <assignment-expression> ::= IDENT = <conditional-expression>
 */
void assignment_expression(){
  entryMsg("assignment_expression");
  //identifier matches here
  match("assignment_expression");

  lex();
  if (nextToken != ASSIGN_OP){
    error("could not parse <assignment_expression>");
  }
  match("assignment_expression");
    
 
  //check for terminials in conditional expression
  lex(); //nextToken needs to be one of the 4 terminals below for it to be a valid conditional expression
  if (nextToken != IDENT && nextToken !=INT_LIT && nextToken !=FLOAT_LIT && nextToken != LEFT_PAREN){
    error("could not parse <assignment_expression>");
  }
  
  conditional_expression();

  exitMsg("assignment_expression");
}

/* Function conditional_statement
   Parses strings in the language generated by the rule:
   <conditional-statement> ::= if ( <conditional-expression> ) <statement> [ else <statement> ]
*/
void conditional_statement(){

  entryMsg("conditional_statement");
 
  if(nextToken != IF_KEY){ 
    error("Could not parse <conditional_statement>");
  }
  match("conditional_statement");
  
  lex();
  if(nextToken != LEFT_PAREN){
    error("Could not parse <conditional_statement>");
  }
  match("conditional_statement");

  lex();
  conditional_expression();

  //nextToken is already read from conditional expression
  if (nextToken != RIGHT_PAREN){
    error("Could not parse <conditional_statement>");
  }

  lex();
  statement();

  if (nextToken == ELSE_KEY){
    match("conditional_statement");
    lex();
    statement();
  }

  exitMsg("conditional_statement");
}

/*Function conditional_expression
  Parses string in the language generated by the rule
  <conditional-expression> ::= <logical-or-expression>
 */
void conditional_expression(){
 
  entryMsg("conditional_expression");
  //once we get to this point, this will pass no matter what
  logical_or_expression();
  //lex() is called at end of logical or
  exitMsg("conditional_expression");  
}

/*Function logical_or_expression
  Parses string in the language generated by the rule
  <logical-or-expression> ::= <logical-and-expression> [ || <logical-or-expression> ]
*/
void logical_or_expression(){
  
  entryMsg("logical_or_expression");

  logical_and_expression(); //returns nextToken
  
  //optional logical or expression                                                                                                                                                 
  if (nextToken==OR_OP){
    match("logical_or_expression");
    lex();
    logical_or_expression();
  }

  exitMsg("logical_or_expression");
}

/*Function logical_and_expression
  Parses string in the language generated by the rule
  <logical-and-expression> ::= <equality-expression> [ && <logical-and-expression> ]
 */
void logical_and_expression(){

  entryMsg("logical_and_expression");
  
  equality_expression();
  
  //optional logical and expression                                                                                                                                                
  if (nextToken==AND_OP){
    match("logical_and_expression");
    lex();
    logical_and_expression();
  }
  exitMsg("logical_and_expression");
}

/*Function equality_expression
  Parses strings in the language generated by the rule
  <equality-expression> ::= <relational-expression> [ <equality-op> <equality-expression> ]
 */
void equality_expression(){
  entryMsg("equality_expression");

  relational_expression();

  //relational expression returns with nextToken already taken care of 
  //optional equality op then equality expression
  if (nextToken == EQUAL_OP || nextToken == NOTEQ_OP){
    match("equality_expression");
    lex();
    equality_expression();
  }
  
  exitMsg("equality_expression");
}

/*Function relational_expression
  Parses string in the language generated by the rule
  <relational-expression> ::= <additive-expression> [ <relational-op> <relational-expression> ]
*/
void relational_expression(){
  entryMsg("relational_expression");

  additive_expression();
  //returns with nextToken
  
  //optional relational op then relational expression
  if (nextToken == LESSTHAN_OP || nextToken == GREATERTHAN_OP || nextToken == LTEQ_OP || nextToken == GTEQ_OP){
    match("relational_expression");
    lex();
    relational_expression();
  }

  exitMsg("relational_expression");
}

/*Function additive_expression
  Parses strings in the language generated by the rule
  <additive-expression> ::= <multiplicative-expression> [ <add-op> <additive-expression> ]
 */
void additive_expression(){
  entryMsg("additive_expression");

  multiplicative_expression();
  //returns with nextToken

  //optional add op and additive expression
  if (nextToken == ADD_OP || nextToken == SUB_OP){
    match("additive_expression");
    lex();
    additive_expression();
  }
  exitMsg("additive_expression");
}

/*Function multiplicative_expression
  Parses strings in the language generated by the rule
  <multiplicative-expression> ::= <primary-expression> [ <mult-op> <multiplicative-expression> ]
 */
void multiplicative_expression(){
  entryMsg("multiplicative_expression");

  primary_expression();

  lex();
  if (nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP){
    match("multiplicative_expression");
    lex();
    multiplicative_expression();
  }

  exitMsg("multiplicative_expression");
}

/*Function primary_expression
  Parses strings in the language generated by the rule 
  <primary-expression> ::= IDENT | <constant>
           | <parenthesized-expression>
 */
void primary_expression(){
  entryMsg("primary_expression");
  if (nextToken == IDENT){
    match("primary_expression");
  }
  else if (nextToken == INT_LIT || nextToken == FLOAT_LIT){
    entryMsg("constant");
    match("constant");
    exitMsg("constant");
  }
  else if (nextToken == LEFT_PAREN){
    parenthesized_expression();
  }
  else {
    error("could not parse <primary_expression>");
  }

  exitMsg("primary_expression");
}

/*Function parenthesized_expression
  Parses string in the language generated by the rule 
  <parenthesized-expression> ::= ( <conditional-expression> )
*/
void parenthesized_expression(){
  entryMsg("parenthesized_expresssion");

  //nextToken assigned in primary expression
  if (nextToken != LEFT_PAREN){
    error("Could not parse <parenthesized_expression>");
  }
  match("parenthesized_expression");

  lex();
  conditional_expression(); 
  //returns with nextToken
  
  if (nextToken != RIGHT_PAREN){
    error("could not parse <parenthesized_expression>");
  } 
  match("parenthesized_expression");

  exitMsg("parenthesized_expression");
}

/*Function iterative_statement
  Parses strings in the language generated in the language
  <iterative-statement> ::= <while-statement> | <for-statement>
 */
void iterative_statement(){
  entryMsg("iterative_statement");
  
  if (nextToken == WHILE_KEY){
    while_statement();
  }
  else if (nextToken == FOR_KEY){
    for_statement();
  }
  else{
    error("could not parse <iterative_statement>");
  }

  exitMsg("iterative_statment");
}

/*Function while_statement
  Parses strings in the language generated in the language
  <while-statement> ::= while ( <conditional-expression> ) <statement>
 */
void while_statement(){
  entryMsg("while_statement");
  match("while_statment");

  lex();
  if (nextToken != LEFT_PAREN){
    error("could not parse <conditional_expression>");
  }
  match("while_statment");

  lex();
  conditional_expression(); //returns with nextToken

  if (nextToken != RIGHT_PAREN){
    error("could not parse <conditiona_expression>");
  }
  match("while_statment");

  lex();
  statement();

  exitMsg("while_statment");
}

/*Function for_statement
  Parses strings in the language generated in the language
  <for-statement> ::= for <for-expressions> <statement>
 */
void for_statement(){
  entryMsg("for_statement");
  match("for_statement");

  for_expressions();

  lex();
  statement();

  exitMsg("for_statement");
}

/*Function for_expressions 
  Parses strings in the language generated in the language
<for-expressions> ::= ( <assignment-expression> ; <conditional-expression> ; <assignment-expression> )
 */
void for_expressions(){
  entryMsg("for_expressions");
  lex();

  if (nextToken != LEFT_PAREN){
    error("could not parse <for_expressions>");
  }
  match ("for_expressions");

  lex();
  if (nextToken != IDENT){
    error("could not parse <for_expressions>");
  }
  assignment_expression();


  if (nextToken != SCOLON_TERM){
    error("could not parse <for_expressions>");
  }
  match ("for_expressions");

  lex();
  conditional_expression();

  //nextToken is read from conditional expression

  if (nextToken != SCOLON_TERM){
    error("could not parse <for_expressions>");
  }
  match ("for_expressions");

  lex();
  if (nextToken != IDENT){
    error("could not parse <for_expressions>");
  }
  assignment_expression();
  
  if (nextToken != RIGHT_PAREN){
    error("could not parse <for_expressions>");
  }
  match ("for_expressions");

  exitMsg("for_expressions");
}

/* Function expr 
   Parses strings in the language
   generated by the rule:
   <expr> => <term> {(+ | -) <term>}
 */

void expr() {

  entryMsg("expr");
  
  /* Parse the first term */
 
  term(); 
  /* As long as the next token is + or -, call 
     lex to get the next token and parse the 
     next term */
  
  while (nextToken == ADD_OP || nextToken == SUB_OP) {
    match("expr");
    lex();  // "consume" the + or -
    term();  
  }

  exitMsg("expr");

}

/* term
   Parses strings in the language generated by the rule:
   <term> => <factor> {(* | /) <factor>)
*/

void term() {
  
  entryMsg("term");

  /* Parse the first factor */
  factor();
  
  /* As long as the next token is * or /,
     next token and parse the next factor */
  while (nextToken == MULT_OP || nextToken == DIV_OP) {
    match("term");
    lex();  // consume * or /
    factor();
  }

  exitMsg("term");

} /* End of function term */


/* Function factor
   Parses strings in the language
   generated by the rule: 
   <factor> => id  | int-constan| ( <expr> )  
*/

void factor() {
  
  entryMsg("factor");

  /* Determine which RHS */
  if (nextToken == IDENT || nextToken == INT_LIT) {
    
    match("factor");
    /* For the RHS id, just call lex */
    lex();  // consume the identifier or int literal
  
  }
  /* If the RHS is (<expr>) – call lex to pass over the left parenthesis, 
     call expr, and check for the right parenthesis */
  else if (nextToken == LEFT_PAREN) {
    match("factor");
    lex();  // consume the (
    expr();
    if (nextToken == RIGHT_PAREN) {
      match("factor");
      lex();  // consume the (
    }
    else
      error("Expected ')' in <factor>");
  }  /* End of else if (nextToken == ...  */
  
  else 
    error("Could not parse <factor>"); /* Neither RHS matches */

  exitMsg("factor");
}

/* if an error is encountered, print a message and exit */
void error(char *msg) {

  printf("PARSE ERROR! %s\n", msg);
  exit(1);
}

/* indentation */
void indent() {

  int i;
  for (i=0; i<level-1; i++) {
    printf("| ");
  }
  if (level > 0) printf("+-");

}

/* print a message that we have entered a particular function
   and increment level so any functions called before we exit
   will be indented */
void entryMsg(char *function) {

  indent();

  printf("<%s> entry\n", function);

  level++;
}

/* print a message that we are exiting a particular function
   and decrement level to restore indentation */
void exitMsg(char *function) {

  level--;

  printf("<%s> exit\n", function);
}

/* print a message that a token was matched */
void match(char *rule) {

  indent();

  printf("Match: %s Token: %2d (%s) in <%s>\n",
	 lexeme, nextToken, tokenCode(nextToken), rule);

}
