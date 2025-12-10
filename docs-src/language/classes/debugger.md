The **Debugger** class allows user to debug an ObjectTalk script.
The class is instantiated in the global context (under the name **debug**)
and can be used to enter the debugger or get state information while running.

	debug(); // enter the interactive debugger by 'executing' the debug instance

When the interactive debugger state in entered, it depends on the
script launch method to determine what happens next. If a script
was started from the command line, a command line based debugger
is activated. If the script was launched from the IDE, a visual
debugger is activated.

It is also possible not to interrupt program execution and get information
about the program state. This is done by executing the following member functions
on the debug instance.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| where() | Return the current execution location as a string. |
| disassemble() | Disassemble the current function and return it as a string. |
| getVariableNames() | Return the variable in use at the current context as an array of strings. |
