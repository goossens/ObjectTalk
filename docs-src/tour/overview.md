Tradition suggests that the first program in a new language should print
the words “Hello, world!” on the screen. In ObjectTalk, this can be done
in a single line:

	print("Hello, world!");

If you have written code in C or any of the derived languages before, this
syntax might look familiar to you. In ObjectTalk, this line of code is a
complete program. You don’t need to import a separate library for
functionality like input/output or string handling. Code written at module
scope is used as the entry point for the program, so you don’t even need
a main() function.

So where do we go from here. Well, we hope the overview below gives you
enough information to start writing code in ObjectTalk by showing you how
to accomplish a variety of programming tasks. Don’t worry if you don’t
understand something, everything introduced in this overview is explained
in detail in the rest of the documentation.

## Everything is an Object

In ObjectTalk, everything is an Object derived from a Class. The number 1
is an object derived from the Integer class and so are 3.14 (derived from
the Real class) and "test" (derived from the String class). To take this
a step further, operators like +, -, *, /, [] and () are actually member
functions on an Object. This keeps the Virtual Machine (VM) very simple as
it has no mathematical instructions. The VM actually only executes member
functions on Objects.

To ensure this doesn't lead to silly syntax, the ObjectTalk compiler
translates more traditional expressions into a set of member function calls.
So for instances, the follow example shows this translation:

	1 + (2 * 3)

	2.__mul__(3).__add__(1)

As a user of ObjectTalk you only have to learn the top syntax. If you want
to understand the compiler and VM, you have to understand both.

Given that operators become member function calls, custom Classes
can implement operators so it is very simple in ObjectTalk to implement
a Vector class that implements a dot product using '*' (__mul__) operator.

## Variables

In ObjectTalk, variables must be declared in the appropriate scope
through assignment. ObjectTalk makes no distinction between constants
and variables. If you set it and don't change it, it's a constant.
If you do change it, it's a variable. The example below creates two
new variables. The first (a) is initialized to the integer 1. The second
(b) is initialized to null as no value was provided and the third (c)
is only visible in the inner scope.

	var a = 1;
	var b;

	{
		var c = 2;
	}

## Primitive Values

ObjectTalk supports 6 [Primitives](classes.html#primitive)
that are derived from the
[Object](classes.html#object), [Boolean](classes.html#boolean),
[Integer](classes.html#integer), [Real](classes.html#real),
[String](classes.html#string) and [Function](classes.html#function)
classes. The following statements assign primitive values to variables.

	var a = null;
	var b = true;
	var c = 1;
	var d = 3.14 * 2;
	var e = "Hello world";
	var f = function() { print("Hello, world!"); }

Please also note that primitive values are automatically converted in
ObjectTalk when required.

	var g = e + c;

In the expression above c gets converted to a String as the addition
operator is applied to e (a String object) which wants its
argument also as a string.

In ObjectTalk, The **null** object is the only instance of the abstract
[Object](classes.html#object) class. It's a special object as it can't
do anything. It is however helpful as a non-value. The default global
scope defines the variable **null** as a convenience.

[Booleans](classes.html#boolean) in ObjectTalk can only hold two values:
true and false. The default global language context defines the variables
**true** and **false** as a convenience.

[Integers](classes.html#integer) represent negative and positive whole
numbers. On most systems, this number is implemented as a 64-bit number
meaning that the extremes are -9,223,372,036,854,775,808 and
9,223,372,036,854,775,807 (inclusive).

[Reals](classes.html#real) in ObjectTalk are double-precision floating
point numbers. On most systems, Reals are implemented using 8 bytes
and have a range of 1.7E +/- 308 (15 digits). The default global language
context predefines the variable **pi** (3.14...) and **e** (2.71...)
as a convenience.

In ObjectTalk, [Strings](classes.html#string) are captured in double
quotes and can span multiple lines. Strings may contain UTF-8 characters
(like "€") or use JSON style encoding like "Most Europeans like the
\u00C4.\n".

[Functions](classes.html#function) in ObjectTalk are primitive values.
This means that we can pass a function around like any other primitive.
The following code might be a little too complex for this overview, so
we'll defer an explanation until we'll get to the
[Language Guide](guide.html).
The point is that a function is a first class citizen in ObjectTalk.

	function test1(callback) {
		callback();
	}

	var test2 = function() {
		print("Hello, world!");
	};

	test1(test2);
	// prints "Hello, world!"

## Collections

ObjectTalk supports three types of
[Collections](classes.html#collections)
([Arrays](classes.html#array),
[Dictionaries](classes.html#dict) and
[Sets](classes.html#set))
that can be nested if required.

[Arrays](classes.html#array) are sequences of objects that can be
constructed using square brackets ([]) or through the Array class
constructor. The Array class has many member functions to manipulate
the content of an array.

	var array1 = [ 1, 2, "test", 7 + 4, a ];
	var array2 = Array(3.14, 7, 34, 1);

	array1.append("new value");
	var thirdValue = array1[3];

[Dictionaries](classes.html#dict) contain indexed key/value pairs that
are constructed using curly brackets ({}) or through the Dict class
constructor. The Dict class also has many member functions to manipulate
the content of a dictionary.

	var dict1 = [ "First Name": "John", "Last Name": "Doe", "Age": 34 };
	var dict2 = Dict("Name", "John Doe", "Address", "Unknown");

	dict1["Last Update"] = "1 Apr 2000";
	var name = dict2["Name"];

Both arrays and dictionaries follow JSON rules and the ObjectTalk
compiler can therefore ingest JSON without any trouble.
This should also make it easier for those who are already familiar
with JSON encoding.

[Sets](classes.html#set) store distinct values in a collection without
defined order. You can use a set instead of an array when the order of
items isn’t important, or when you need to ensure that an item only
appears once. Sets can be created using the Set class constructor.
The Set class also has many member functions to manipulate the content
of a set or to perform set operations (e.g. union, difference).

	var set = Set(1, 2, 3, 5);
	var set2 = Set(1, 3, 6, 8);

	assert(set.intersection(set2) == Set(1, 3));
	assert(set.difference(set2) == Set(2, 5, 6, 8));
	assert(set.union(set2) == Set(1, 2, 3, 5, 6, 8));
	assert(set.subtract(set2) == Set(2, 5));

## Control Flow

ObjectTalk uses **if** to make conditionals and uses **for in**,
**while** and **do while** to make loops. Parentheses around the
condition or loop variables are optional. Braces around the body are required.

	var individualScores = [75, 43, 103, 87, 12];
	var teamScore = 0;

	for score in individualScores {
		if score > 50 {
			teamScore += 3;

		} else {
			teamScore += 1;
		}
	}

	print(teamScore);
	// Prints "11"

In ObjectTalk, conditions are converted to their boolean value in
**if**, **while** and **do while** statements. In the following example,
the string and integer variables are converted to a boolean and assessed
in the **if** statements.

	var A = 0;
	var B = 5;
	var C = "true";

	if A {
		print("this will never be printed as variable A evaluates to false");
	}

	if B {
		print("variable B evaluated to true");
	}

	if C {
		print("variable C evaluated to true");
	}

In ObjectTalk **switch** or **case** type statements are implemented
with **if**, **elif** and **else** constructs. This keeps the syntax
clean and it allows more powerful expressions for each case (e.g.
regular expressions).

	if (a == 1) {
		print("1");

	} elif (a == 2) {
		print("2");

	} elif (a == 3) {
		print("3");

	} else {
		print("it's something else");
	}

## Functions

In ObjectTalk, there are two way to declare a function. First, there is
the traditional way that you see in many languages. The second is to
create an anonymous function that can be assigned to a variable. In
ObjectTalk, the following two are the same:

	function test(var1) {
		return var1;
	}

	var test = function(var1) {
		return var1;
	});

Later we'll talk about member functions in Object and Classes but their
syntax is basically the same.

Functions can be nested. Nested functions have access to variables that
were declared in the outer function. You can use nested functions to
organize the code in a function that’s long or complex.

	function returnFifteen() {
		var y = 10;

		function add() {
			y += 5;
		}

		add();
		return y;
	}

	returnFifteen();

Functions are a first-class type. This means that a function can return another function as its value.

	function makeIncrementer() {
		function addOne(number) {
			return number + 1;
		}

		return addOne;
	}

	var increment = makeIncrementer()
	print(increment(7));
	// prints 8

A function can take another function as one of its arguments.

	function hasAnyMatches(list, condition) {
		for item in list {
			if condition(item) {
				return true;
			}
		}

		return false;
	}

	function lessThanTen(number) {
		return number < 10;
	}

	var numbers = [20, 19, 7, 12];
	print(hasAnyMatches(numbers, lessThanTen));
	// prints true

Functions are actually a special case of closures: blocks of code that can
be called later. The code in a closure has access to things like variables
and functions that were available in the scope where the closure was
created, even if the closure is in a different scope when it’s executed.
You saw an example of this already with nested functions.

## Object and Classes

Use **class** followed by the class’s name, a colon and the parent class'
name to create a new class. A property declaration in a class is written
the same way as a constant or variable declaration, except that it’s in
the context of a class. Likewise, member function and function
declarations are written the same way.

	class Shape : Object {
		function simpleDescription(this) {
			return "A simple shape"
		}
	}

Create an instance of a class by putting parentheses after the class name
(executing the call operator(\_\_call\_\_)). Use dot syntax to access the
properties and member function of the instance.

	var shape = Shape();
	shape.numberOfSides = 7;
	var shapeDescription = shape.simpleDescription();

This version of the Shape class is missing something important: an
initializer (constructor) to set up the class when an instance is created.
Use \_\_init\_\_ to create one.

	class NamedShape : Shape {
		function __init__(this, name) {
			this.name = name;
		}

		function simpleDescription(this) {
			return "A named shape called " + this.name;
		}
	}

Notice how "this" is used to distinguish the name property from the name
argument to the initializer. In some computer language, the "this" or
"self" argument is automatically added to a member function. In
ObjectTalk this is not the case and you have to name it yourself
as the first parameter. The benefit is that you can call it whatever you
want and it's not some hidden feature of the language. The disadvantage
is that you have to type it for every member function. The compiler
automatically translates a bound function (a function in the context of
an object) to a new function call with an additional parameter.

	a.test(1, 2) becomes test(a, 1, 2);

## Error Handling

ObjectTalk uses a classic **try/catch/throw** paradigm. Critical code
is captured in a **try** block and if an exception is raised, the code
in the **catch** block is executed. If an exception is raised outside
a try block, your script will terminate.

A simple example might look like:

	try {
		var a = 1;
		var b = 2;
		var c = 4 / 0;

	} catch error {
		print("Caught an exception: ", error);
	}

	print("Recovered from exception");

In the example above, the ObjectTalk runtime will raise the exception
as the variable **c** is initialized through a "divide by zero"
computation.

Programmers can also use the **throw** command to raise an exception:

	try {
		throw "this is completely wrong";

	} catch error {
		print("Caught an exception: ", error);
	}

	print("Recovered from exception");

In the catch block, the error message is automatically assigned to
the variable mentioned after the **catch** statement.
