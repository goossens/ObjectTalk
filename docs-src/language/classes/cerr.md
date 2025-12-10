The Cerr class represents the stderr stream and allow you to send text to the console.
The [Global](#global) scope pre-defines an Cerr instance under the name **cerr**. Example:

	cerr << "Hello World";

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_lshift__(operand) | Convert operand to a string and send it to stderr. |
