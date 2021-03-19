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
that can be accessed with the dot (member) operator. Metadata is not
preserved beyond the running of a script and should therefore not be
used as a dictionary. For that, please of the [Dict](Dict.md) class.
Metadata is powerful as any data can be attached to an object.

When metadata is retrieved, the ObjectTalk engine for looks in the
metadata attached to the object and if it can't be found,
it's class and parent classes are searched. This is actually the
mechanism by which class member functions (methods) are found.

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
	a.flag = true;            // only set metadata on object a

	a.dosomething = function(this) {
		return "I did it";
	};                        // store a function in object's a metadata

	assert(a.flag == true);
	assert(a.dosomething() == "I did it");

	result = false;
	c = "";

	try {
		c = b.dosomething();  // this won't work a b does not have metadata
	}

	catch error {
		result = true;        // we expected this error
	}

	assert(result == true);
	assert(c == "");

	String.flag2 = true;      // add metadata to String class

	String.dosomething = function(this) {
		return "I did it";
	};                        // add function to String class metadata

    // now the metadata is available on on String objects
	assert(a.flag2 == true);
	assert(b.flag2 == true);
	assert(a.dosomething() == "I did it");
	assert(b.dosomething() == "I did it");

Class Methods
-------------

| Method | Description |
| ------ | ----------- |
| boolean() | Convert object to a boolean. This is a virtual function that child classes must overwrite if required. By default, this function returns **false**. |
| integer() | Convert object to an integer. This is a virtual function that child classes must overwrite if required. By default, this function returns **0**. |
| real() | Convert object to a real. This is a virtual function that child classes must overwrite if required. By default, this function returns **0.0**. |
| string() | Convert object to a string.  This is a virtual function that child classes must overwrite if required. By default, this function returns **""**. |
| json() | Represent object in JSON format.  This is a virtual function that child classes must overwrite if required. By default, this function returns the **string** representation. |
| has(name) | See if object has named metadata. |
| \_\_member__(name) | Return a [MemberReference](MemberReference.md) object to acces an object's metadata. This method is called when you use the **member** (.) operator. You typically don't call this method directly. |
| eraseMember(name) | Erase named metadata. |
| clearMembers() | Erase all metadata. |
| \_\_eq__(object) | See if objects are equal. This virtual method is called when you use the **equal** (==) operator and should be overwritten when required. You typically don't call this method directly. |
| \_\_ne__(object) | See if objects are not equal. This virtual method is called when you use the **not equal** (!=) operator and should be overwritten when required. You typically don't call this method directly. |
| getClass() | return the object's class. |
| isKindOf(className) | See if object is derived from specified class. |
