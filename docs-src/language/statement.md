![Railroad Diagram](img/statement.png)

	statement ::= expression ";"
	| block
	| "var" name ("=" expression)? ";"
	| "class" name ":" expression block
	| "function" name "(" ((name ("," name)*)?) ")" block
	| "if" expression block ("elif" expression block)* ("else" block)?
	| "while" expression block
	| "do" block "while" expression ";"
	| "for" name "in" expression block
	| "throw" expression ";"
	| "try" block "catch" name block
	| "return" expression ";"
