The Internal class is an abstract class without any functionality derived
from [Object](Object.md). The purpose of it in ObjectTalk is simply to
group all internal classes and to provide a common base class for them.

Internal classes can't be instantiated directly and are simply there to help
ObjectTalk internally. Classes like iterators, references and others
are used internally to ensure the entire engine is object based. These
classes are never exposed to the user. Three internally classes are the
exception. **Class**, **Global** and debugger can't be instantiated but users
can interact with these classes.
