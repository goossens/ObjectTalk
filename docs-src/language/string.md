![Railroad Diagram](img/string.png)

    string ::= '"' (Char - ["\] | '\' ["\bfnrt] | "\u"  [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f] [0-9A-Fa-f])* '"'
