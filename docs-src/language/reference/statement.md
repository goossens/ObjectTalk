![Railroad Diagram](img/statement.png)

	statement ::= expression ";"
	| block
	| "var" identifier ("=" expression)? ";"
	| "class" identifier ":" expression block
	| "function" identifier "(" ((identifier ("," identifier)*)?) ")" block
	| "if" expression block ("elif" expression block)* ("else" block)?
	| "while" expression block
	| "do" block "while" expression ";"
	| "for" identifier "in" expression block
	| "throw" expression ";"
	| "try" block "catch" identifier block
	| "return" expression ";"
