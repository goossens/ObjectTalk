# ObjectTalk in EBNF

Below is a formal expression of the ObjectTalk scripting language in
Extended Backus–Naur Form (EBNF). You can read more about this notation
on [Wikipedia](https://en.wikipedia.org/wiki/Extended_Backus–Naur_form). The pretty diagrams were created using the
["RailRoad Diagram Generator"](https://www.bottlecaps.de/rr/ui) which
translates EBNF into HTML with pictures. This tool is highly recommended
for visualizing grammar.

# Module
![Railroad Diagram](ebnf/module.png)

    module ::= statement*

# Statement
![Railroad Diagram](ebnf/statement.png)

    statement ::= expression
    | block
    | "class" name ":" expression block
    | "class" name ":" expression block
    | "class" name ":" expression block
    | "function" name "(" (name ("," name)* | "...") ")" block
    | "if" expression block ("else" block)?
    | "while" expression block
    | "do" block "while" expression
    | "for" name "in" expression block
    | "switch" expression "{" ("case" expression statement)+ ("default" statement)? "}"
    | "return" expression

# Block
![Railroad Diagram](ebnf/block.png)

    block ::= "{" statement+ "}"

## expression
![Railroad Diagram](ebnf/expression.png)

    expression ::= conditional ("=" conditional | "*=" conditional | "/=" conditional | "%=" conditional |
    "+=" conditional | "-=" conditional | "|=" conditional | "^=" conditional | "&=" conditional)*

# expressions
![Railroad Diagram](ebnf/expressions.png)

    expressions ::= expression ("," expression)*

## conditional
![Railroad Diagram](ebnf/conditional.png)

    conditional ::= or ("?" expression ":" expression)?

## or
![Railroad Diagram](ebnf/or.png)

    or ::= and ("||" and)*

## and
![Railroad Diagram](ebnf/and.png)

    and ::= bytewise-or ("&&" bytewise-or)*

## bytewise-or
![Railroad Diagram](ebnf/bytewise-or.png)

    bytewise-or ::= bytewise-xor ("|" bytewise-xor)*

## bytewise-xor
![Railroad Diagram](ebnf/bytewise-xor.png)

    bytewise-xor ::= bytewise-and ("^" bytewise-and)*

## bytewise-and
![Railroad Diagram](ebnf/bytewise-and.png)

    bytewise-and ::= equal ("&" equal)*

## equal
![Railroad Diagram](ebnf/equal.png)

    equal ::= relation ("==" relation  | "!=" relation)*

## relation
![Railroad Diagram](ebnf/relation.png)

    relation ::= shift ("<" shift | "<=" shift | ">" shift | ">=" shift | "in" shift | "not in" shift)*

## shift
![Railroad Diagram](ebnf/shift.png)

    shift ::= addition ("<<" addition | ">>" addition)*

## addition
![Railroad Diagram](ebnf/addition.png)

    addition ::= multiplication ("+" multiplication | "-" multiplication)*

## multiplication
![Railroad Diagram](ebnf/multiplication.png)

    multiplication ::= prefix ("*" prefix | "/" prefix | "%" prefix)*

## prefix
![Railroad Diagram](ebnf/prefix.png)

    prefix ::= ("-" | "!" | "^" | "++" | "--")? postfix

## postfix
![Railroad Diagram](ebnf/postfix.png)

    postfix ::= primary ("[" expression "]" | "(" expressions ")" | "." name | "++" | "--")*

## primary
![Railroad Diagram](ebnf/primary.png)

    primary ::= "(" expression ")"
    | "[" expressions "]"
    | "{" string ":" expression ("," string ":" expression)* "}"
    | integer
    | real
    | string
    | function
    | name

## integer
![Railroad Diagram](ebnf/integer.png)

    integer ::= [-]? [0-9][0-9]*

## real
![Railroad Diagram](ebnf/real.png)

    real ::= [-]? [0-9][0-9]*.[0-9][0-9]* ([Ee] [-]? [0-9][0-9]*)?

## string
![Railroad Diagram](ebnf/string.png)

    string ::= '"' (Char - ["\] | '\' ["\bfnrt] | "\u"  [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f])* '"'

## function
![Railroad Diagram](ebnf/function.png)
    function ::= "function" "(" (name ("," name)* | "...") ")" statement

## name
![Railroad Diagram](ebnf/name.png)

    name ::= [a-zA-Z_][a-zA-Z0-9_]*
