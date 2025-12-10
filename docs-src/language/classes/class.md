The **Class** class is an internal class that provides information
about another class. This class can't be instantiated but users
can request access to this class by using the **getClass** member
function on any object.

	var object = "dsfdsfdsf";
	var class = object.getClass();
	print(class.getName());

**Member Functions**

| Function | Description |
| ------ | ----------- |
| getName() | Returns the name of the class. |
| hasParent() | Returns a Boolean to indicate if the class has a parent class.. |
| getParent() | Returns the parent class. |
| isKindOf(className) | Returns a Boolean indicating if the class is derived from another class. |
