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
but in the end it does hurt as modern concepts like capture for
callback (explained later) are still supported, it keeps the
implementation easier to understand and we can keep variable
assignment without declaration. Finding a variable is a runtime
decision not a compile time decision.

ObjectTalk implments 4 different context types:

* **Global Context** - When the ObjectTalk interpreter is started,
a default global scope is created. This is the root scope and contains
a small set of useful variables (null, true, false), a few high level
functions (import, print, and assert) and the list of classes that can be
instantiated by the user. For a detailed description see the documentation
on the [Global class](../reference/Global.md).
* **Module Scope** - An ObjectTalk [module](Modules.md) is a context
object.
