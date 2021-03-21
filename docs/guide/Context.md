# Context and Scope

In ObjectTalk, variables are stored in a context chain. When a variable
is stored, it will always go to the current context. When a variable
is retrieved, the context chain is consulted from the current context
back to the root.

ObjectTalk uses dynamic scoping (a detailed explanation can be found on
[Wikipedia](https://en.wikipedia.org/wiki/Scope_(computer_science)).
In simple terms, in dynamic scoping the runtime first searches the
current context and then successively all the calling functions.
This may sound strange in a modern language since it is very rare
but in the end it does hurt as modern concepts like closure or capture
are still supported, it keeps the implementation easier to understand
and we can keep variable assignment without declaration. Most importantly,
given that ObjectTalk is object-oriented first, it is better to let
variables be class or object members rather than rely on complex scoping
rules.

Finding a variable is a runtime decision not a compile time decision.
This is not the most efficient way as some case could be handled at
compile time but it makes the engine simpler and easier to understand.

ObjectTalk implements 4 different context types:

* **Global Context** - When the ObjectTalk interpreter is started,
a default global scope is created. This is the root scope and contains
a small set of useful variables (null, true, false), a few high level
functions (import, print, and assert) and the list of classes that can be
instantiated by the user. For a detailed description see the documentation
on the [Global class](../reference/Global.md).
* **Module Scope** - An ObjectTalk [module](Modules.md) is a context
object.
