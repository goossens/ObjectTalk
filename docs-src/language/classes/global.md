The **Global** class can't be instantiated but is created by the ObjectTalk runtime
and it represents the top-level or root scope. Members of the **Global** class
can be accessed anywhere in the ObjectTalk runtime. Right now, the global scope
contains frequently used constants, global functions, class definitions for the
core classes and instances of core classes as a convenience.

**Global Constants**

| Variable | Description |
| ------ | ----------- |
| null | Instance of the **Object** class representing a none (null) value. |
| true | Boolean true. |
| false | Boolean false. |
| pi | Mathematical constant that is the ratio of a circle's circumference to its diameter. |
| e | Mathematical constant that is the base of the natural logarithm and exponential function. |

**Global Functions**

| Function | Description |
| ------ | ----------- |
| assert(expression) | Evaluates the expression and terminates the program with an error if false. |
| addImportPath(path) | Add the specified path to the import search path. Helpful to find module in non-standard locations |
| import(module) | Import a module into the current context. The module can refer to an internal module or a path to a user module. |
| range() | Returns a number iterator. If called with one parameter, the iterator yields to number from 0 to n-1. If called with two parameters, the iterator yields number from the first to the last. If called with three parameters, the first indicates the start, the second indicates the end and the third indicates the step. Step in this case may be a negative number to create a reverse number iterator. |
| print(text[, ...]) | Prints each value to stdout as well as a linefeed. |
| members() | Returns the members of an object. If the object is a **Class** class, it returns the class members. |

**Global Classes**

The global scope contains all the core classes so core objects can be easily instantiated. The following classes are predefined:
- **Primitives:** Object, Boolean, Integer, Real, String, Function
- **Collections:** Array, Dict, Set
- **System:** Path, IO, OS, FS
- **Streams:** Cout, Cerr

**Global Instances**

As a convenience, the following classes have instances in the global scope:

- **io:** IO instance
- **os:** OS instance
- **fs:** FS instance
- **cout:** Cout instance
- **cerr:** Cerr instance
- **debug:** Debugger instance
