grammar lingua;

translation_unit: module_preamble global_declaration*;

// Modules
module_preamble: module_declaration import_declaration*;
module_declaration: 'module' module_identifier ';';
import_declaration: 'import' module_identifier ';';
module_identifier: IDENTIFIER ('.' IDENTIFIER)*;

// Declarations
global_declaration: 'export'? declaration;

declaration: type_declaration ';'
           | function_declaration ';'
           | variable_declaration ';'
           ;

type_declaration: struct_declaration
                | enum_declaration
                ;
struct_declaration: 'type' IDENTIFIER '{' global_declaration* '}';
enum_declaration: 'enum' IDENTIFIER '{' identifier_list '}';

function_declaration: 'fun' IDENTIFIER '(' parameter_list ')' return_type? compound_statement;
return_type: '->' type;
parameter_list: parameter_declaration (',' parameter_declaration)*;
parameter_declaration: IDENTIFIER ':' type;

variable_declaration: 'let' IDENTIFIER (':' type)? initialiser;
initialiser: '<-' expression;

// Types
type: type_modifier? ref_modifier? type_specifier;
type_specifier: 'bool'
              | 'char8'
              | 'int8'
              | 'int16'
              | 'int32'
              | 'int64'
              | 'float16'
              | 'float32'
              | 'float64'
              | 'string'
              | 'void'
              ;
type_modifier: 'readable'
             | 'writable'
             | 'mutable'
             ;
ref_modifier: 'ref';

// Expressions
expression: assignment_expression;
assignment_expression: logical_or_expression  (ASSIGNMENT_OPERATOR assignment_expression)?;
logical_or_expression: logical_and_expression ('or'  logical_or_expression)?;
logical_and_expression: equality_expression   ('and' logical_and_expression)?;
equality_expression: inequality_expression (EQUALITY_OPERATOR equality_expression)?;
inequality_expression: additive_expression (INEQUALITY_OPERATOR inequality_expression)?;
additive_expression: multiplicative_expression (ADDITIVE_OPERATOR additive_expression)?;
multiplicative_expression: unary_expression (MULTIPLICATIVE_OPERATOR multiplicative_expression)?;
unary_expression: unary_operator? primary_expression;
unary_operator: '+'
              | '-'
              | '++'
              | '--'
              | 'addressof'
              | 'copy'
              | 'not'
              | 'sizeof'
              | 'valueof'
              ;
primary_expression: '(' expression ')'
                  | IDENTIFIER postfix_expression?
                  | BOOLEAN_LITERAL
                  | CHARACTER_LITERAL
                  | FLOATING_LITERAL
                  | INTEGRAL_LITERAL
                  | STRING_LITERAL
                  ;
postfix_expression: '.' IDENTIFIER postfix_expression?
                  | '[' expression ']'
                  | '(' argument_list? ')'
                  ;

argument_list: argument (',' argument)*;
argument: expression;

// Statements
statement: compound_statement
         | selection_statement
         | iteration_statement
         | assert_statement
         | break_statement
         | continue_statement
         | expression_statement
         | return_statement
         | declaration_statement
         ;
compound_statement: '{' statement* '}';
assert_statement: 'assert' expression ';';
break_statement: 'break' ';';
continue_statement: 'continue' ';';
expression_statement: expression ';';
return_statement: 'return' expression? ';';
declaration_statement: variable_declaration ';';

// Selection statements
selection_statement: if_statement;
if_statement: 'if' expression compound_statement;

// Iteration statements
iteration_statement: while_statement
                   | for_statement
                   ;
while_statement: 'while' expression compound_statement;
for_statement: 'for' parameter_declaration 'in' IDENTIFIER compound_statement;

// Miscellaneous
identifier_list: IDENTIFIER (',' IDENTIFIER)*;

// Terminal tokens
fragment ESCAPED_QUOTE : '\\"';
MODULE_SEPARATOR: [.];
BOOLEAN_LITERAL: 'false' | 'true';
IDENTIFIER: [A-Za-z][A-Za-z0-9_]*;

CHARACTER_LITERAL: '\'' (ESCAPED_QUOTE | ~('\n' | '\r')) '\'';
FLOATING_LITERAL: [0-9]+'.'[0-9]+([Ee][+-]?[0-9]+)?;
INTEGRAL_LITERAL: [0-9]+;
STRING_LITERAL : '"' ( ESCAPED_QUOTE | ~('\n'|'\r') )*? '"';

EQUALITY_OPERATOR: '=' | '!=';
INEQUALITY_OPERATOR: [<>] '='?;
ASSIGNMENT_OPERATOR: '<-';
ADDITIVE_OPERATOR: [+-];
MULTIPLICATIVE_OPERATOR: [*/%];

WS: [ \t\r\n]+ -> skip;
