/*
 * Grammar file for the Calculator application.
 */
grammar Calculator; 

program             : (exprs+=expression ';')+ EOF ;

booleanConstant     : val=(TRUE | FALSE) ;
assignExpression    : <assoc=right> v=VARIABLE ':=' ex=expression ;

expression          : 
                   '(' ex=expression ')'                              # ParenExpr
                    | <assoc=right> op='-' ex=expression               # UnaryMinusExpr
                    | <assoc=right> op='~' ex=expression               # UnaryNotExpr
                    | left=expression op=('*' | '/') right=expression  # BinaryArithExpr
                    | left=expression op=('+' | '-') right=expression  # BinaryArithExpr
                    | left=expression op=('<' | '>') right=expression  # BinaryRelExpr
                    | <assoc=right> 
                      left=expression op=('=' | '~=') right=expression # EqExpr
                    | assignExpression                                # AsgExpr
                    | booleanConstant                                 # BConstExpr 
                    | v=VARIABLE                                      # VariableExpr 
                    | i=INTEGER                                       # IConstExpr
                    ;

// Lexer rules
// Operators
ASSIGN    : ':=' ;
DIVIDE    : '/' ;
EQUAL     : '=' ;
GREATER   : '>' ;
LESS      : '<' ;
MINUS     : '-' ;
MULTIPLY  : '*' ;
NOT       : '~' ;
PLUS      : '+' ;
UNEQUAL   : '~=' ;

// Separators (punctuation)
LPAR      : '(' ;
RPAR      : ')' ; 
SEMICOLON : ';' ;

// Boolean constants
FALSE     : 'false' ;
TRUE      : 'true' ;

// Other
INTEGER   : [0-9]+ ;
VARIABLE  : [a-zA-Z]+ ;

WS         : [ \n\t\r]+ -> skip ;
