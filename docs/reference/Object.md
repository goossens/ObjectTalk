# Object Class

ObjectTalk uses single inheritance to simplify the language and
every class must be derived from another class. The only exception
to this rule is the Object class as it provides the foundation for
all objects. As such, all ObjectTalk classes are derived (directly or
indirectly) from Object. The Object class is the only class without
a parent class.

Given that the Object class is very abstract, it is not very
meaningful to instantiate it. Off course there is an exception here
as well as the default global context creates a **null** object
that can be used to denote a value that does not exist.

	a = null;
	b = Object();
	assert(a == b);

In the example above, both **a** and **b** have the same (non) value.

All ObjectTalk objects have a special mechanism to store metadata
that can be accessed with the dot operator. Metadata is not preserved
beyond the running of a script and should therefore not be used
as a dictionary. For that, please of the [Dict](Dict.md) class.
Metadata is powerful as any data can be attached to an object.

When metadata is retrieved, the ObjectTalk engine for looks in the
metadata registry attached to the object and if it can't be found,
it's class and parent classes are consulted. This is actually the
mechanism by which data and function members are found.

	a = -1;
	a.flag = true;            // attach metadata to object a
	assert(a.flag == true);
	assert(a.abs() == 1);     // abs member function from Integer class

	Integer.flag2 = false;    // set metadata on Integer class
	assert(a.flag2 == false); // find metadata in Integer object

This metadata power however comes with a disadvantage as you can create
really obscure code. Here is an example:

	a = "test";
	b = "test";
	a.flag = true;

	a.dosomething = function(this) {
		return "I did it";
	};

	assert(a.flag == true);
	assert(a.dosomething() == "I did it");

	result = false;
	c = "";

	try {
		c = b.dosomething();
	}

	catch error {
		result = true;     // we expected this error
	}

	assert(result == true);
	assert(c == "");

	String.dosomething = function(this) {
		return "I did it";
	};

	assert(a.flag == true);
	assert(b.flag == true);
	assert(a.dosomething() == "I did it");
	assert(b.dosomething() == "I did it");

Class Methods
-------------

| Method | Description |
| ------ | ----------- |
| boolean() | Force conversion of object to a boolean value |
| integer() | Force conversion of object to an integer value |
| real() | Force conversion of object to a real value |
| string() | Force conversion of object to a string value |
| json() | Represent object in JSON format |
| has(name) | See if object has named metadata |
| \_\_member__(name) | Return object's named metadata (this method is called when you use the index ([]) operator) |
| eraseMember(name) | Erase named metadata |
| clearMembers() | Erase all metadata |
| \_\_eq__(object) | See if object are equal (this method is called when you use the equal (==) operator) |
| \_\_ne__(object) | See if object are not equal (this method is called when you use the not equal (!=) operator) |
| getClass() | return the object's class |
| isKindOf(className) | See if object is derived from specified class |

Examples
--------
