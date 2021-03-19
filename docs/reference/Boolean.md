# Boolean Class

Booleans in ObjectTalk can only hold two values: true and false. The
default global language context defines the variables **true** and
**false** as a convenience.

Class Methods
-------------

| Method | Description |
| ------ | ----------- |
| \_\_and__(boolean) | Perform a logical **and** with the boolean representation of the operand. This method is called when you use the **and** (&&) operator. |
| \_\_or__(boolean) | Perform a logical **or** with the boolean representation of the operand. This method is called when you use the **or** (\|\|) operator. |
| \_\_not__() | Take truth to falsity and vice versa. This method is called when you use the **not** (!) operator. |

Overrides
---------

The Boolean class overrides the following methods from the Object class:

* boolean
* integer
* real
* string
* \_\_eq__
* \_\_ne__
