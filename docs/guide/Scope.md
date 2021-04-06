# Scope

Scope is a concept that refers to where objects including functions and
classes can be seen and accessed. In ObjectTalk, variables are declared and stored in a scope. The scope of a variable describes where in a program's
text the variable may be used, while the extent or lifetime describes when
in a program's execution a variable has a value.

ObjectTalk uses lexical scoping (a detailed explanation can be found on
[Wikipedia](https://en.wikipedia.org/wiki/Scope_(computer_science)).
In lexical scoping, name resolution depends on the location in the source
code and the lexical context, which is defined by where the named object
is defined.

ObjectTalk implements 5 different scope types:

* **Global Scope** - When the ObjectTalk interpreter is started,
a default global scope is created. This is the root scope and contains
a small set of useful variables (null, true, false), a few high level
functions (import, print, and assert) and the list of classes that can be
instantiated by the user. For a detailed description see the documentation
on the [Global class](../reference/Global.md). Global scope members
are stored on the heap.
* **Module Scope** - An ObjectTalk [module](Modules.md) is derived from
a single source code file. Any object defined at the module's root level
is added to the object scope. The effect this achieves is that those
objects have global visibility in that module and can simply be addressed
by their name. If a module is imported into another module, the variables
of the imported module can be accessed by the importer using the member
notation (dot operator). Module scope members are stored on the heap.
* **Class Scope** - In ObjectTalk, classes represent a separate scope.
Any object (variables, functions, subclasses) defined in a class become
part of the class scope. These object can be addressed inside the class
by their name and using the member notation (dot operator) outside of the
class. Class scope members are stored on the heap.
* **Function Scope** - In ObjectTalk, functions represent a separate scope.
Unlike the previous 3 types, function scope objects live on the stack as
their lifetime tends to be short. The function scope includes the calling
parameters.
* **Block Scope** - In ObjectTalk, every block (code contained in
curly brackets({})). has it's own scope. Variables contained in blocks
are stored on the stack as the are also very short lived.

ObjectTalk does have a mechanism to capture variables from a parent
scope using a closure mechanism. The special mechanism which is often
required in lambda function or just because functions are first class
citizens in ObjectTalk, is described in in the [closure](Closure.md)
section.
