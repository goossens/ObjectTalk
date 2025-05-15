The **Debugger** class allows user to debug an ObjectTalk script.
The class is instantiated in the golbal context (under the name **debug**)
and can be used to enter the debugger or get state information while running.

	debug(); // enter the inteactive debugger.

When the interactive debugger state in entered, it depends on the
script launch method to determine what happens next. If a script
was started from the command line, a command line based debugger
is activated. If the script was launched from the IDE, a visual
debugger is activated.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| debug() | Enter the interactive debugger. |
| where() | Return the current execution location as a string. |
| disassemble() | Disassemble the current function and return it as a string. |
| getVariableNames() | Return the variable in use at the current context as an array of strings. |
