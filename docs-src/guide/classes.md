Classes are general-purpose, flexible constructs that become the building
blocks of your program’s code. You define properties and member functions
to add functionality to your classes using the same syntax you use to
define variables and functions.

An instance of a class is traditionally known as an object. However, in
this ObjectTalk documentation the terms object and instance are used
interchangeably.

**Definition Syntax**

You introduce classes with the class keyword:

	class Resolution : Object {
	    var width = 0;
	    var height = 0;
	}

The example above defines a new class called Resolution (that is derived
from the Object class), to describe a pixel-based display resolution. This
class has two class variables called width and height. Class variables are
variables that are stored as part of the class. All instances of Resolution
share these variables. To have unique variables per instances we need to
assign values to the instance member.

	class Resolution : Object {
		// instance member initialized in constructor
		function__init__(this, width, height) {
			this.width = 0;
		    this.height = 0;
		}
	}

	class VideoMode : Object {
		var classVariable = "mode class";

		function__init__(this) {
		    this.resolution = Resolution(1920, 1080);
		    this.interlaced = false;
		    this.frameRate = 0.0;
		    this.name;
		}
	}

	var mode = VideoMode();
	mode.name = "Test Mode"; // assign value to instance variable

The example above also defines a new class called VideoMode, to describe a
specific video mode for video display. This class has four instance
variables. The first, resolution, is initialized with a new Resolution
instance, which infers a property type of Resolution. For the other
three properties, new VideoMode instances will be initialized with an
interlaced setting of false (meaning “noninterlaced video”), a playback
frame rate of 0.0, and an optional String value called name. The name
property is automatically given a default value of **null**.

**Class Instances**

The Resolution  and the VideoMode classes definition only describe what a
Resolution or VideoMode will look like. They themselves don’t describe a
specific resolution or video mode. To do that, you need to create an
instance of the structure or class.

	var someResolution = Resolution(1920, 1080);
	var someVideoMode = VideoMode();

The simplest form of initializer syntax uses the type name of the class
followed by empty parentheses, such as VideoMode(). This creates a new
instance of the class. In some cases (like the Resolution class above),
the class constructor (\_\_init__) can take additional parameters to
initialize a class instance.

**Accessing Members**

You can access the members of an instance using dot syntax. In dot syntax,
you write the property name immediately after the instance name, separated
by a period (.):

	print("The width of someResolution is ", someResolution.width);
	// Prints "The width of someResolution is 1920"

In this example, someResolution.width refers to the width property of
someResolution, and returns its default initial value of 0.

You can drill down into sub-properties, such as the width property in the
resolution property of a VideoMode:

	print("The width of someVideoMode is ", someVideoMode.resolution.width");
	// Prints "The width of someVideoMode is 1920"

You can also use dot syntax to assign a new value to a variable property:

	someVideoMode.resolution.width = 1280;
	print("The width of someVideoMode is ", someVideoMode.resolution.width");
	// Prints "The width of someVideoMode is 1280"

The dot notations can be used to access both instance and class member
variables. When reading a member, the runtime will first look in the
instance and if it can't be found the runtime will search the class and
all it's parent classes. Here is an example:

	class Root : Object {
		var value0 = 0;
		var value1 = 1;
	}

	class Child : Root {
		var value0 = false;
	}

	class GrandChild : Child {
		var value2 = 2;
		var value3 = 3;
	}

	var grandChild = GrandChild();
	grandChild.value3 = true;

	assert(grandChild.value0 == false);
	assert(grandChild.value1 == 1);
	assert(grandChild.value2 == 2);
	assert(grandChild.value3 == true);

When setting a member, the dot notation can be used to set instance and
class members like:

	grandChild.age = 5; // setting an instance member
	GrandChild.value3 = null; // setting a class member;

**Member Functions**

Member functions are associated with classes and encapsulate specific
tasks and functionality for working with an instance of a given class.

You write an member functions within the opening and closing braces of the
class they belongs to. A member function can be called only on a specific
instance of the class it belongs to.

Here’s an example that defines a simple Counter class, which can be used
to count the number of times an action occurs:

	class Counter : Object {
		function__init__(this) {
		    this.count = 0;
		}

	    function increment(this) {
	        this.count += 1;
	    }
	    function increment(this, amount) {
	        this.count += amount;
	    }
	    function reset(this) {
	        this.count = 0;
	    }
	}

The Counter class defines three instance methods:
* increment(this) - increments the counter by 1.
* increment(this, amount) - increments the counter by a specified amount.
* reset(this) - resets the counter to zero.

You call instance methods with the same dot syntax as properties:

var counter = Counter(); // the initial counter value is 0
counter.increment();     // the counter's value is now 1
counter.increment(5);    // the counter's value is now 6
counter.reset();         // the counter's value is now 0

**The this Property**

Notice in the examples above how "this" is used to distinguish the name
property from the name argument to the initializer. In some computer
language, the "this" or "self" argument is automatically added to a member
function. In ObjectTalk this is not the case and you have to name it
yourself as the first parameter of a member function. The benefit is that
you can call it whatever you want and it's not some hidden feature of the
language. The disadvantage is that you have to type it for every member
function. The compiler automatically translates a bound function (a
function in the context of an object) to a new function call with an
additional parameter.

	counter.increment(2); // becomes:
	Counter.increment(counter, 2);

**Constructor**

Initialization is the process of preparing an instance of a class for use
and it's performed by the class' constructor. This process involves
setting an initial value for member variables on that instance and
performing any other setup or initialization that’s required before
the new instance is ready for use.

You implement this initialization process by defining constructors,
which are like special member functions that can be called to create a
new instance of a particular type. Their primary role is to ensure that
new instances of a type are correctly initialized before they’re used for the first time.

The name of the constructor is always **\_\_init__** as you can see in the
examples above. Here's another example:

	class Fahrenheit : Object {
	    function __init__(this) {
	        this.temperature = 32;
	    }
	}

	var f = Fahrenheit();
	print("The default temperature is ", f.temperature, "° Fahrenheit")
	// Prints "The default temperature is 32° Fahrenheit"

In case the class is derived from a class that also has a constructor,
the superclass constructor must also be called like:

	class Temperature : Object {
		function __init__(this) {
			this.measurement = "temperature";
		}
	}

	class Fahrenheit : Temperature {
		function __init__(this) {
			Temperature.__init__(this);
			this.temperature = 32;
		}
	}

**Operators**

In ObjectTalk, everything is an Object derived from a Class. The number 1
is an object derived from the Integer class and so are 3.14 (derived from
the Real class) and "test" (derived from the string class). To take this
a step further, operators like +, -, *, /, [] and () are actually member
functions of a class.

To ensure this doesn't lead to silly syntax, the ObjectTalk compiler
translates more traditional expressions into a set of member function calls.
So for instances, the follow example shows this translation:

	1 + (2 * 3)

	2.__mul__(3).__add__(1)

As a user of ObjectTalk you only have to learn the top syntax. If you want
to understand the compiler and VM, you have to understand both.

Given that operators become member function calls, custom Classes
can implement operators so it is very simple in ObjectTalk to implement
a Vector class that implements a dot product operator.

You can find a list of all operator function member name in the
[Operators](#operators) section.

**Subclassing**

You can subclass any class as ObjectTalk supports the concept of single
inheritance (meaning a class can derive from a single other class). In fact
all classes in ObjectTalk have a parent class with the exception of the
**Object** class which is the root class for all others.

	class A : Object {
	    function someMethod(this) {
			print("A.someMethod()");
		}
	}

	class B : A {
	    function someMethod(this) {
			print("B.someMethod()");
		}
	}

It’s valid for a subclass member to call a superclass member using the
global **super** function:

	class A : Object {
		function someMethod(this, value) {
			print("A.someMethod(", value, ")");
		}
	}

	class B : A {
		function someMethod(this, value) {
			super(this, "someMethod", value);
			print("B.someMethod(", value, ")");
		}
	}

	var b = B();
	b.someMethod(2);
