Strings in ObjectTalk are sequences of characters and are captured in
double quotes and can span multiple lines. Strings may contain UTF-8
characters (like "€") or use JSON style encoding like
"Most Europeans like the \u00C4.\n".

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_gt__(string) | Compares string to the object's value alphabetically and returns a boolean indicating whether the object's value is greater than the integer or not. This member function is called when you use the **greater than** (>) operator. |
| \_\_lt__(string) | Compares string to the object's value alphabetically and returns a boolean indicating whether the object's value is less than the integer or not. This member function is called when you use the **less than** (<) operator. |
| \_\_ge__(string) | Compares string to the object's value alphabetically and returns a boolean indicating whether the object's value is greater or equal than the integer or not. This member function is called when you use the **greater or equal than** (>=) operator. |
| \_\_le__(string) | Compares string to the object's value alphabetically and returns a boolean indicating whether the object's value is less or equal than the integer or not. This member function is called when you use the **less or equal than** (<=) operator. |
| casecmp(string) | Compares string to the object's value alphabetically but case independently and returns an integer indicating whether the object's value is less, equal or greater than the string. |
| \_\_index__(integer) | Return a [StringReference](StringReference.md) object to address individual characters in a string. This member function is called when you use the **index** ([]) operator. You typically don't call this member function directly. |
| \_\_iter__() | Return a [StringIterator](StringIterator.md) object to iterate through individual characters in a string. This member function is called when you use the **for in** statement. You typically don't call this member function directly. |
| \_\_add__(string) | Concatenates string to the object's value and returns result. This member function is called when you use the **addition** (+) operator. |
| \_\_contains__(string) | Determine if string is contained in objects value. This member function is called when you use the **(not) in** operators. |
| len() | Return number of characters in object's value. |
| left(len) | Return the left most number of characters in object's value. If number is higher than length of string, the entire string is returned. |
| right(len) | Return the right most number of characters in object's value. If number is higher than length of string, the entire string is returned. |
| mid(pos, len) | Return the portion of the object's value that starts at character position pos and spans len characters (or until the end of the string, whichever comes first). |
| find(string) | Searches the object's value for the first occurrence of the specified string and return its position. If string is not found, -1 is returned. |
| startsWith(string) | Determines if the object's value starts with the specified string. A Boolean is returned. |
| contains(string) | Determines if the object's value contains the specified string. A Boolean is returned. |
| trim() | Trim all whitespaces (SPACE, TAB, VTAB, CR, LF) from the start and the end of the object's value and return result. |
| ltrim() | Trim all whitespaces (SPACE, TAB, VTAB, CR, LF) from the start of the object's value and return result. |
| rtrim() | Trim all whitespaces (SPACE, TAB, VTAB, CR, LF) from the end of the object's value and return result. |
| compress() | Trim all whitespaces (SPACE, TAB, VTAB, CR, LF) from the start and the end of the object's value, replace whitespace sequence in the middle with a single space and return result. |
| lower() | Convert all characters in the object's value to lowercase and return result. |
| upper() | Convert all characters in the object's value to uppercase and return result. |
| split(delimiter) | Split string based on delimiter and return array of substrings. |
| format(parameters...) | Format a string using the value as a mask and the parameters to substitute (like printf). |

**Overrides**

The String class overrides the following member functions from the Object class:

* boolean
* integer
* real
* string
* json
* \_\_eq__
* \_\_ne__
