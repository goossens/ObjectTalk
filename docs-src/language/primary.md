![Railroad Diagram](img/primary.png)

    primary ::= "(" expression ")"
    | "[" expressions "]"
    | "{" string ":" expression ("," string ":" expression)* "}"
    | integer
    | real
    | string
    | function
    | name
