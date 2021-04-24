Integers in ObjectTalk represent negative and positive whole numbers.
On most systems, this number is implemented as a 64-bit number
meaning that the extremes are -9,223,372,036,854,775,808 and
9,223,372,036,854,775,807 (inclusive).

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_gt__(integer) | Compares integer to the object's value and returns a boolean indicating whether the object's value is greater than the integer or not. This member function is called when you use the **greater than** (>) operator. |
| \_\_lt__(integer) | Compares integer to the object's value and returns a boolean indicating whether the object's value is less than the integer or not. This member function is called when you use the **less than** (<) operator. |
| \_\_ge__(integer) | Compares integer to the object's value and returns a boolean indicating whether the object's value is greater or equal than the integer or not. This member function is called when you use the **greater or equal than** (>=) operator. |
| \_\_le__(integer) | Compares integer to the object's value and returns a boolean indicating whether the object's value is less or equal than the integer or not. This member function is called when you use the **less or equal than** (<=) operator. |
| \_\_add__(integer) | Add integer to object's value and return result. This member function is called when you use the **add** (+) operator. |
| \_\_sub__(integer) | Subtract integer from object's value and return result. This member function is called when you use the **subtract** (-) operator. |
| \_\_mul__(integer) | Multiple integer with object's value and return result. This member function is called when you use the **multiply** (*) operator. |
| \_\_div__(integer) | Divide object's value by integer and return result. This member function is called when you use the **divide** (/) operator. |
| \_\_mod__(integer) | Divide object's value by integer and return leftover. This member function is called when you use the **modulo** (%) operator. |
| \_\_pow__(integer) | Raise object's value to the power of integer and return result. This member function is called when you use the **exponent** (\*\*) operator. |
| \_\_inc__() | Return object's value + 1. This member function is called when you use the **increment** (++) operator. |
| \_\_dec__() | Return object's value - 1. This member function is called when you use the **decrement** (--) operator. |
| \_\_lshift__(integer) | Shift bits in object's value left by integer and return result. This member function is called when you use the **shift left** (<<) operator. |
| \_\_rshift__(integer) | Shift bits in object's value right by integer and return result. This member function is called when you use the **shift right** (>>) operator. |
| \_\_band__(integer) | Perform a bitwise logical AND between object's value and integer and return result. This member function is called when you use the **bitwise and** (&) operator. |
| \_\_bor__(integer) | Perform a bitwise logical OR between object's value and integer and return result. This member function is called when you use the **bitwise or** (|) operator. |
| \_\_bxor__(integer) | Perform a bitwise logical exclusive OR between object's value and integer and return result. This member function is called when you use the **bitwise exclusive or** (^) operator. |
| \_\_bnot__() | Perform logical negation of each bit in object's value and return result. This member function is called when you use the **bitwise and** (~) operator. |
| \_\_neg__() | Perform a unary negation of object's value and return result. This member function is called when you use the **unary negation** (-) operator. |
| abs() | Return absolute value of object's value.
| sign() | Return sign (-1, 0 or 1) of object's value.

**Overrides**

The Integer class overrides the following member functions from the Object class:

* boolean
* integer
* real
* string
* \_\_eq__
* \_\_ne__
