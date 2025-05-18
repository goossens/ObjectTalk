Functions are self-contained chunks of code that perform a specific task.
ObjectTalk has three function "types". First there is the traditional
function as you would define it in other languages. Secondly there is the
"lambda" version of a function which makes a function look more like
any other primitive. In fact, the only difference between these first
two types is syntax. The third type is the member function which
is a function attached to a class and performs a task on an instance of
that class. This is what they looks like:

	// traditional function definition
	function callMeTraditional(parameter) {
		return parameter + 1;
	}

	// "lambda" style function
	var callMeLambda = function(parameter) {
		return parameter + 1;
	}

	// member function
	class newestObjectClass : Object {
		var increment = 1;

		function callMeMemberFunction(this, parameter) {
			return parameter + this.increment;
		}
	}

Traditional and lambda functions will be described in this section.
Member Functions are covered in the [Classes](#classes) section.

When you define a function, you can optionally define one or more named,
values that the function takes as input, known as parameters.
Function parameters and return values are extremely flexible in ObjectTalk.
You can define anything from a simple utility function with a single
parameter to a complex function with expressive parameters.

**Functions Without Parameters**

Functions aren’t required to define input parameters. Here’s a function with
no input parameters, which always returns the same String message whenever it’s called:

	function sayHelloWorld() {
		return "hello, world";
	}

	print(sayHelloWorld());
	// Prints "hello, world"

The function definition still needs parentheses after the function’s name,
even though it doesn’t take any parameters. The function name is also
followed by an empty pair of parentheses when the function is called.

**Functions With Multiple Parameters**

Functions can have multiple input parameters, which are written within
he function’s parentheses, separated by commas.

This function takes a person’s name and whether they have already been
greeted as input, and returns an appropriate greeting for that person:

	function greet(person, alreadyGreeted) {
		if (alreadyGreeted) {
			return greetAgain(person);

		} else {
			return greet(person);
		}
	}

	print(greet("Tim", true));
	// Prints "Hello again, Tim!"

**Functions Without Return Values**

Functions aren’t required to return a value. Here’s a version of the
greet function, which prints its own String value rather than
returning it:

	function greet(person) {
		print("Hello, ", person, "!");
	}

	greet("Dave");
	// Prints "Hello, Dave!"

Strictly speaking, this version of the greet() function does still return
a value, even though no return value is defined. Functions without a defined
return will automatically return **null**.

**Functions as Return Value**

You can use a function as the return value of another function.
The next example defines two simple functions called stepForward() and
stepBackward(). The stepForward() function returns a value one more than
its input value, and the stepBackward() function returns a value one less
than its input value.

	function stepForward(input) {
		return input + 1;
	}

	function stepBackward(input) {
		return input - 1;
	}

Here’s a function called chooseStepFunction() which returns the stepForward()
function or the stepBackward() function based on a Boolean parameter called backward:

	function chooseStepFunction(backward) {
		return backward ? stepBackward : stepForward;
	}

You can now use chooseStepFunction() to obtain a function that will step
in one direction or the other:

	var currentValue = 3;
	var moveNearerToZero = chooseStepFunction(currentValue > 0);
	// moveNearerToZero now refers to the stepBackward() function

The example above determines whether a positive or negative step is needed
to move a variable called currentValue progressively closer to zero.
currentValue has an initial value of 3, which means that currentValue > 0
returns true, causing chooseStepFunction(backward:) to return the
stepBackward() function. A reference to the returned function is stored
in a constant called moveNearerToZero.

Now that moveNearerToZero refers to the correct function, it can be used
to count to zero:

	print("Counting to zero:");

	// Counting to zero:
	while (currentValue != 0) {
		print(currentValue, "... ");
		currentValue = moveNearerToZero(currentValue);
	}

	print("zero!");
	// 3...
	// 2...
	// 1...
	// zero!

**Nested Functions**

All of the functions you have encountered so far in this chapter have been
examples of global functions, which are defined at a global or module
scope. You can also define functions inside the bodies of other functions,
known as nested functions.

Nested functions are hidden from the outside world by default, but can
still be called and used by their enclosing function. An enclosing
function can also return one of its nested functions to allow the nested
function to be used in another scope.

You can rewrite the chooseStepFunction() example above to use and return nested functions:

	function chooseStepFunction(backward) {
		function stepForward(input) { return input + 1 }
		function stepBackward(input) { return input - 1 }
		return backward ? stepBackward : stepForward;
	}

	var currentValue = -4;
	var moveNearerToZero = chooseStepFunction(currentValue > 0);

	// moveNearerToZero now refers to the nested stepForward() function
	while (currentValue != 0) {
		print(currentValue, "...");
		currentValue = moveNearerToZero(currentValue);
	}

	print("zero!");

	// -4...
	// -3...
	// -2...
	// -1...
	// zero!
