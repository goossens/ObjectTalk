The IO class provides methods to read and write object and text to and from a file.
The [Global](#global) scope pre-defines an IO instance under the name **io**.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| readJSON(file) | Read a specified JSON file and return ObjectTalk objects. |
| writeJSON(file, object) | Write ObjectTalk object to a specified JSON file. |
| readText(file) | Read the specified file and return it as a String object. |
| writeText(file, object) | Convert the specified object to a String and write it to the specified file. |
