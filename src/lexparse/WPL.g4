/*
 * Alex Friedman (ahfriedman@wpi.edu)
 * Grammar file for WPI Programming Language (WPL).
 */
grammar WPL;

// Parser rules
compilationUnit   :  (stmts+=statement | extens+=externStatement | defs+=defineType)* EOF ; 

structCase        :  (ty=type name=VARIABLE) ';' ;

defineType        : 'define' 'enum' name=VARIABLE LSQB cases+=type (',' cases+=type)+ '}' # DefineEnum
                  | 'define' 'struct' name=VARIABLE LSQB (cases+=structCase)*  RSQB       # DefineStruct
                  ; 

externStatement : EXTERN (ty=type FUNC | PROC) name=VARIABLE LPAR ((paramList=parameterList variadic=VariadicParam?)? | ELLIPSIS) RPAR ';';

invocation          :  (field=fieldAccessExpr | lam=lambdaConstExpr) LPAR (args+=expression (',' args+=expression)* )? RPAR ;

fieldAccessExpr     : fields+=VARIABLE ('.' fields+=VARIABLE)*  ;
//Helps allow us to use VARIABLE or arrayAccess and not other expressions (such as for assignments)
arrayAccess         : field=fieldAccessExpr '[' index=expression ']'; 
arrayOrVar          : var=VARIABLE | array=arrayAccess  ;

/*
 * Expressions return values. These can be: 
 *      1. Parenthetical expressions of the form: ( expr )
 *      2. Unary Expressions such as: -1 and ~false
 *      3. Binary Arithmetic expressions such as: 1 * 2, 6 / 3, 2 + 3, 5 - 6
 *              Note: Separated into two lines for presedence of * and / over + and - 
 *      4. Binary Relational expressions such as: 1 < 2, 1 <= 2, 2 > 1, 2 >= 1
 *      5. Equality expressions such as : 1 = 2, true ~= false
 *      6. Logical AND such as: true & true
 *      7. Logical OR such as: true | false
 *      8. Invocations of functions (as they have return values)
 *      9. Field Access such as: array.length
 *              Note: this uses an expression instead of a variable because, in theory, 
 *              an extended language could allow for something like (<condition> ? array1 : array2).length
 *      10. Array accesses such as: array[0]. 
 *              Note: While theoreticaly, a language could allow for something like:
 *               (<condition> ? array1 : array2)[(<condition> ? array 1 : array2).length - 1]
 *              because WPL does not appear to have this functionality, it is easier to require
 *              the use of variables instead of expressions for array access
 *      11-14. Typical boolean and variable constants. 
 */
expression          : LPAR ex=expression RPAR                       # ParenExpr
                    | fieldAccessExpr                               # FieldAccess
                    | <assoc=right> op=(MINUS | NOT) ex=expression  # UnaryExpr 
                    | left=expression op=(MULTIPLY | DIVIDE) right=expression # BinaryArithExpr
                    | left=expression op=(PLUS | MINUS) right=expression      # BinaryArithExpr
                    | left=expression op=(LESS | LESS_EQ | GREATER | GREATER_EQ) right=expression # BinaryRelExpr 
                    | <assoc=right> left=expression op=(EQUAL | NOT_EQUAL) right=expression # EqExpr
                    | exprs+=expression (AND exprs+=expression)+     # LogAndExpr 
                    | exprs+=expression (OR  exprs+=expression)+     # LogOrExpr
                    | call=invocation                               # CallExpr
                    | v=VARIABLE '::init' '(' (exprs+=expression (',' exprs+=expression)*)? ')' # InitProduct
                    | arrayAccess  # ArrayAccessExpr
                    | booleanConst # BConstExpr 
                    | i=INTEGER    # IConstExpr
                    | s=STRING     # SConstExpr 
                    | lambdaConstExpr # LambdaExpr
                    ;

lambdaConstExpr     : LPAR parameterList RPAR ':' ret=type block ;

/* 
 * Keeping block as its own rule so that way we can re-use it as
 * its own statement as well as as parts of functions, procedures, 
 * conditions, loops, etc. 
 */
block      : LSQB (stmts+=statement)* '}' ;


// Parenthesis are optional around a condition
condition           : (LPAR ex=expression RPAR) | ex=expression ; 

//Used to model each alternative in a selection 
selectAlternative   : check=expression ':' eval=statement ; 
matchAlternative    : check=type name=VARIABLE '=>' eval=statement ;


/*
 * Helps to consistently manage parameters. 
 * 
 * ParameterList allows to get a list of one parameter
 *      followed by any number of subsequent parameters that are 
 *      comma separated.
 *
 * Parameter defines a parameter: its just a type and name.
 */
parameterList          : params+=parameter (',' params+=parameter)*? ;
parameter           :  ty=type name=VARIABLE ;
VariadicParam : ',' [ \t]* '...'; //For some reason, need to match the whitespace so that way we can allow spaces between the two...

/*
 * Assignment fragment: this contains the information about variables
 * that we wish to declare as well as a potential expression to serve as their 
 * value. This is used by the VarDeclStatement to help separate multiple 
 * assigments. Ie those of the form:   var a <- 1, b <- 2, ... 
 */
assignment : v+=VARIABLE (',' v+=VARIABLE)* (ASSIGN ex=expression)? ; //FIXME: NEEDS TO BE FIELD ACCESS?



/* 
 * Statements do not return values but are still necessary to the language. 
 * The statements are: 
 * 1. Definition of external functions/procedures. 
 * 2. Definition of functions
 * 3. Definition of procedures
 * 4. Assignments (updates to existing variables) such as: a <- 2; 
 * 5. Variable definitions such as: var a; int [5] b; var a, b; var a, b <- 1; var a, b <- 1, c, d, e <- 2; etc.
 * 6. Looping statements (while loops)
 * 7. Conditional statements (if with optional else)
 * 8. Select statements (which require at least one select alternative)
 * 9. Calls to Procedures (as they do not return a value)
 * 10. Return statements
 * 11. Block statements. 
 */
statement           : ((ty=type FUNC) | PROC) name=VARIABLE LPAR (paramList=parameterList)? RPAR block   # FuncDef 
                    | <assoc=right> to=arrayOrVar ASSIGN ex=expression ';'                  # AssignStatement 
                    | <assoc=right> ty=typeOrVar assignments+=assignment (',' assignments+=assignment)* ';'   # VarDeclStatement
                    | WHILE check=condition DO block                                    # LoopStatement 
                    | IF check=condition IF_THEN? trueBlk=block (ELSE falseBlk=block)?  # ConditionalStatement
                    | SELECT LSQB (cases+=selectAlternative)* '}'                        # SelectStatement  
                    | MATCH check=condition LSQB (cases+=matchAlternative)* '}'          # MatchStatement
                    | call=invocation  ';'?     # CallStatement 
                    | RETURN expression? ';'    # ReturnStatement 
                    | block                     # BlockStatement
                    ;   

//Operators
ASSIGN      :       '<-'    ; 
MINUS       :       '-'     ;
NOT         :       '~'     ;
MULTIPLY    :       '*'     ;
DIVIDE      :       '/'     ;
PLUS        :       '+'     ;
LESS        :       '<'     ;
LESS_EQ     :       '<='    ;
GREATER_EQ  :       '>='    ;
GREATER     :       '>'     ;
EQUAL       :       '='     ;
NOT_EQUAL   :       '~='    ;
AND         :       '&'     ;
OR          :       '|'     ;
MAPS_TO     :       '->'    ;


//Separators
LPAR      :     '('     ;
RPAR      :     ')'     ;
LBRC      :     '['     ;
RBRC      :     ']'     ;
LSQB      :     '{'     ;
RSQB      :     '}'     ;
SEMICOLON :     ';'     ;
COLON     :     ':'     ;
QUOTE     :     '"'     ;
COMMA     :     ','     ;
ELLIPSIS  :     '...'   ;

/* 
 * Types
 */

//Used for when we can either provide a type or a variable (needed bc var arrays are not allowed).
typeOrVar       : type | 'var'  ;

//Allows us to have a type of ints, bools, or strings with the option for them to become 1d arrays. 
type            :    ty=type LBRC len=INTEGER RBRC                                  # ArrayType
                |    ty=(TYPE_INT | TYPE_BOOL | TYPE_STR)                           # BaseType
                |    paramTypes+=type (',' paramTypes+=type)* '->' returnType=type  # LambdaType
                |    LPAR type ('+' type)+ RPAR                                     # SumType 
                |    VARIABLE                                                       # CustomType
                ;

TYPE_INT        :   'int' ; 
TYPE_BOOL       :   'boolean' ;
TYPE_STR        :   'str' ; 

//Others
FUNC            :   'func'  ;
PROC            :   'proc'  ;
IF              :   'if'    ;
IF_THEN         :   'then'  ;
ELSE            :   'else'  ;
WHILE           :   'while' ;
RETURN          :   'return';
SELECT          :   'select';
DO              :   'do'    ;
EXTERN          :   'extern';
MATCH           :   'match' ;


//Booleans
booleanConst        :   TRUE | FALSE ; 
FALSE       :   'false' ; 
TRUE        :   'true'  ; 

//Integer 
INTEGER     :   '0' | [1-9][0-9]* ; //Negative numbers handled by unary minus 

/*
 * Strings
 */

//Strings start with a quote and then contain as many 
//Escaped charaters or safe characters for a string to 
//contain (basically anything except for a \ or a " as these designate
//either the end of the string, or an escape character). 
//Finally, the string is ended by a quote. 
STRING      :   QUOTE (ESCAPE_STRING | SAFE_STRING)* QUOTE;

//Variables 
VARIABLE  :     [a-zA-Z][a-zA-Z0-9_]*  ;

//String escapes are a \ that must be followed by any character
fragment ESCAPE_STRING  : '\\'. ; 
//Without an escape, strings cannot include a quote (that represents end of string)
//nor can they include a slash (that would start an escape)
fragment SAFE_STRING : ~["\\]    ;


/*
    WPL has two types of comments, inline comments and standard comments. 
    Inline comments start with a '#' character that is not in a string 
    literal, and continue to the end of the line (or end of file if this 
    is the last line of the program).

    Standard comments begin with a "(*" sequence and end with a matching 
    "*)" sequence.  Standard comments may be nested.

    Whitespace consists of this set of characters `[ \t\r\n\f]` 
    (not including the brackets). Comments and whitespace are not passed 
    to the parser from the scanner.
 */
 
//Inline comments start with a # and go to the end of line. NOTE: ANTLR uses ~ instead of ^ for negation in regex
INLINE_COMMENT  :   '#' .*? ('\n'|EOF)  -> skip;

/*
 * Regular comments start with (* and go until *) while supporting nesting comments. 
 * To achieve this, after the start of a nested comment with (*, we allow the comment 
 * to contain another regular comment, and we otherwise do not allow them to contain 
 * (* or *). This ensures proper nesting or a syntax error. By then using the non-greedy
 * repitition rule (*?), we make sure that we close out the nested comment on the first valid *)
 * that we encounter. This step might not be needed given the explicit disallow of (* and *)
 * in the comment outside of another comment, but I figure that its probably a good practice
 * to leave it in there.  
 */
STD_COMMENT     :   '(*'  (STD_COMMENT | '('~'*' | ~'*'')' | ~'(')*? '*)' -> skip ;  

//Typical whitespace skip
WS  : [ \t\r\n\f]+ -> skip; 