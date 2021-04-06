# The Basics

ObjectTalk is an object-oriented scripting language that was developed
from the ground up to be efficient and pure. Nonetheless, many parts of
ObjectTalk will be familiar your experience of developing in C and
derived languages.

ObjectTalk provides its own versions of all fundamental C types, including
Booleans, Integers (for int), Reals (for double), String for textual data
and Functions as first class primitives. ObjectTalk also provides powerful
versions of the two primary collection types, Array and Dictionary, as
described in Collection Types. In fact, in ObjectTalk everything is an
object derived from a class and the word type is only used internally
in the language's runtime. Primitives like Booleans, Integers, Reals,
Strings and Functions are objects just like Array, Dict and HTTP servers.

Variables
---------

Variables associate a name (e.g. myVariable) with an object derived from
a certain class (e.g. the number 10 or the string "Hello"). Variables
in ObjectTalk must be declared to establish visibility in a certain
scope. In the following example (let's assume this is at the top level
of a module), the value 10 will be assigned to variable myVariable in the
module's scope.

	var myVariable = 10;
	print(myVariable); // this will print the number 10.

In the following example, the variable is assigned to a class OtContext
and therefore becomes a class member.

	class myClass: Object {
		var myVariable = 10;
	}

	print(myClass.myVariable); // this will print the number 10.

As you can see from the examples, the variables do not have to be type
declared as the assignment  determines what type of object is stored
in the variable.

Comments
--------

In ObjectTalk, you can uses comments to include non-executable text in
your code, as a note or reminder to yourself. Comments are ignored by
the ObjectTalk compiler when your code is compiled. ObjectTalk allows
three types of comments: multiline C-style comments, single line
C++ style comments and single line shell-style comments.

Multiline C-style comments start with a forward-slash followed by an
asterisk:

	/* This is also a comment
       written over multiple lines. */

Single-line C++ style comments begin with two forward-slashes:

	// This is a comment.

Single-line shell-style comments begin with a number sign:

	# This is a comment.

A shell style comment allows ObjectTalk scripts to be made executable
in UNIX type systems. The following first line (typically called a
"shebang") tell the UNIX shell to execute this script with the ObjectTalk
interpreter.

	#!/usr/bin/ot

Semicolons
----------

Unlike some newer languages, ObjectTalk mandates the use of Semicolons
at the end of statements that don't end with a block. This allows
statements to be written over multiple lines (without the need for
backslashes) and it avoids possible ambiguity. For example, earlier
versions of ObjectTalk allowed:

	a = 10
	++b

To most people, it is obvious that we want to assign the number 10 to
variable a and increment variable b. The compiler however doesn't known
the first statement ended after the number 10 and it will generate code
for:

	a = 10++;
	b;

To avoid this confusion, semicolons are mandatory at the end of statements
that don't end with a block so the ObjectTalk script should have read:

	a = 10;
	++b;

Null
----

Null is a special value that indicates a valueless state. All instances
of the Object class all null values.

	var nothing = null;
	var alsoNothing = Object();

Booleans
--------

Booleans in ObjectTalk can only hold two logical values: true and false.
The default global scope defines the variables **true** and **false**
as a convenience. A large number of language operators or member functions
return Booleans as a result.

	var bool1 = true;
	var bool2 = false;
	var bool3 = !bool2;

Integers
--------

Integers are signed whole numbers with no fractional component, such as 42
and -23. On most systems, this number is implemented as a 64-bit number
meaning that the extremes are -9,223,372,036,854,775,808 and
9,223,372,036,854,775,807 (inclusive).

Integer literals can be written as:

* A decimal number, with no prefix
* A binary number, with a 0b or 0B prefix
* An octal number, with a 0o or 0O prefix
* A hexadecimal number, with a 0x or 0X prefix

All the following integer literals have a decimal value of 17:

	var decimalInteger = 17;
	var binaryInteger = 0b10001;       // 17 in binary notation
	var octalInteger = 0o21;           // 17 in octal notation
	var hexadecimalInteger = 0x11;     // 17 in hexadecimal notation

Reals
-----

Reals are signed floating point numbers with a fractional component, such
as 3.14159, 0.1, and -273.15. Reals are implemented using 8 bytes and have
a range of 1.7E +/- 308 (15 digits).

Real literals can be written as a floating point number with or without an exponent:

	var decimalDouble = 12.1875;
	var exponentDouble = 1.21875e1;

Strings
-------

Strings are a series of characters, such as "hello, world" or "albatross".
ObjectTalk strings are instances of the String class. Strings are
made up of unicode characters encoded in UTF-8. Given that UTF-8 uses
variable length encoding, ObjectTalk does not measure or index strings
in bytes but rather in codewords that represent a character whether it is
1,2, 3 or 4 bytes long.

String literals are encoded using the same logic as JSON making it
easy to exchange with other languages.A string literal is a sequence of
characters surrounded by double quotation marks (").

	var someString = "Some string literal value";
	var message = "\tMost Europeans like the \u00C4.\n";

Multiline strings can also be created:

	var quotation = "The White Rabbit put on his spectacles.
		\"Where shall I begin, please your Majesty?\" he asked.

		\"Begin at the beginning,\" the King said gravely,
		\"and go on till you come to the end; then stop.\"";
