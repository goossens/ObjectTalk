# Function Class

Functions in ObjectTalk are primitive values. This means that we can pass
a function around like any other primitive. Function are first class
citizens in ObjectTalk.

In ObjectTalk, there are two types of functions: those that are mapped to
machine level C or C++ code and that that are written in ObjectTalk.
From a usage perspective there is no difference and both are called the
same way, stored the same way and can be passed as arguments. The two
examples below create the same result. In the first case, you see  a
traditional function declaration where as in the second case, an anonymous
(lambda) function is assigned a variable.

	function test() {

	}

	test = function() {

	};

Class methods in ObjectTalk are regular functions and the ObjectTalk
compiler translates a perceived method call into a function call. In the
example below the test member of an object is called with object as
the first parameter. This is why methods (member functions) in their
definition must have a first parameter that refers to the object.
ObjectTalk does not care if you call that first member self, this or
anything else. You can also see in the example that methods (member
functions) can be declared in a traditional way are a lambda assignments.

	object.test(p1, p2); becomes test(object, p1, p2);

	class Test : Object {
		function test(this, p1, p2) {
		}

		function test2(self) {
		}

		test3 = function(object) {
		}
	}

Class Methods
-------------

| Method | Description |
| ------ | ----------- |
| \_\_call__(...) | execute the function and return the result. |
