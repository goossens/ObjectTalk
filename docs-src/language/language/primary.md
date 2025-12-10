![Railroad Diagram](img/primary.png)

	primary ::= "(" expression ")"
	| "[" expression ("," expression)* "]"
	| "{" string ":" expression ("," string ":" expression)* "}"
	| integer
	| real
	| string
	| function
	| name
	| "super" "." name
