![Railroad Diagram](img/string.png)

	string ::= '"' ("\u" [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f] | '\' ["\bfnrt] | [^\\"])* '"'

