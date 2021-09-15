Below is a formal expression of the ObjectTalk scripting language in
Extended Backus–Naur Form (EBNF). You can read more about this notation
on [Wikipedia](https://en.wikipedia.org/wiki/Extended_Backus–Naur_form). The pretty diagrams were created using the
["RailRoad Diagram Generator"](https://www.bottlecaps.de/rr/ui) which
translates EBNF into HTML with pictures. This tool is highly recommended
for visualizing grammar.

## module
![Railroad Diagram](img/module.png)

    module ::= statement*

## statement
![Railroad Diagram](img/statement.png)

	statement ::= expression ";"
	| block
	| "var" name ("=" expression)? ";"
	| "class" name ":" expression block
	| "function" name "(" ((name ("," name)*)? | "...") ")" block
	| "if" expression block ("elif" expression block)* ("else" block)?
	| "while" expression block
	| "do" block "while" expression ";"
	| "for" name "in" expression block
	| "throw" expression ";"
	| "try" block "catch" name block
	| "return" expression ";"

## block
![Railroad Diagram](img/block.png)

    block ::= "{" statement* "}"

## expressions
![Railroad Diagram](img/expressions.png)

    expressions ::= expression ("," expression)*

### expression
![Railroad Diagram](img/expression.png)

    expression ::= conditional (
        "=" expression |
        "*=" expression |
        "/=" expression |
        "%=" expression |
        "+=" expression |
        "-=" expression |
        "|=" expression |
        "^=" expression |
        "&=" expression)*

### conditional
![Railroad Diagram](img/conditional.png)

    conditional ::= or ("?" expression ":" expression)?

### or
![Railroad Diagram](img/or.png)

    or ::= and ("||" and)*

### and
![Railroad Diagram](img/and.png)

    and ::= bytewise-or ("&&" bytewise-or)*

### bytewise-or
![Railroad Diagram](img/bytewise-or.png)

    bytewise-or ::= bytewise-xor ("|" bytewise-xor)*

### bytewise-xor
![Railroad Diagram](img/bytewise-xor.png)

    bytewise-xor ::= bytewise-and ("^" bytewise-and)*

### bytewise-and
![Railroad Diagram](img/bytewise-and.png)

    bytewise-and ::= equal ("&" equal)*

### equal
![Railroad Diagram](img/equal.png)

    equal ::= relation ("==" relation  | "!=" relation)*

### relation
![Railroad Diagram](img/relation.png)

    relation ::= shift (
        "<" shift |
        "<=" shift |
        ">" shift |
        ">=" shift |
        "in" shift |
        "not in" shift)*

### shift
![Railroad Diagram](img/shift.png)

    shift ::= addition ("<<" addition | ">>" addition)*

### addition
![Railroad Diagram](img/addition.png)

    addition ::= multiplication ("+" multiplication | "-" multiplication)*

### multiplication
![Railroad Diagram](img/multiplication.png)

    multiplication ::= prefix ("*" prefix | "/" prefix | "%" prefix)*

### prefix
![Railroad Diagram](img/prefix.png)

    prefix ::= ("-" | "!" | "^" | "++" | "--")? postfix

### postfix
![Railroad Diagram](img/postfix.png)

    postfix ::= primary ("[" expression "]" | "(" expressions ")" | "." name | "++" | "--")*

### primary
![Railroad Diagram](img/primary.png)

    primary ::= "(" expression ")"
    | "[" expressions "]"
    | "{" string ":" expression ("," string ":" expression)* "}"
    | integer
    | real
    | string
    | function
    | name

### integer
![Railroad Diagram](img/integer.png)

    integer ::= [-]? [0-9][0-9]*

### real
![Railroad Diagram](img/real.png)

    real ::= [-]? [0-9][0-9]*.[0-9][0-9]* ([Ee] [-]? [0-9][0-9]*)?

### string
![Railroad Diagram](img/string.png)

    string ::= '"' (Char - ["\] | '\' ["\bfnrt] | "\u"  [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f])* '"'

### function
![Railroad Diagram](img/function.png)

	function ::= "function" "(" ((name ("," name)*)?) ")" block


### name
![Railroad Diagram](img/name.png)

    name ::= [a-zA-Z_][a-zA-Z0-9_]*
