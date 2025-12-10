![Railroad Diagram](img/postfix.png)

	postfix ::= primary ("[" expression "]" | "(" expression ("," expression)* ")" | "." name | "++" | "--")*
