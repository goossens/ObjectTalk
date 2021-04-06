# ObjectTalk Overview

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

## Variables

In ObjectTalk, variables must be declared as in the appropriate scope
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

ObjectTalk supports 6 [Primitive](reference/Primitive.md) values
that are derived from the
[Object](reference/Object.md), [Boolean](reference/Boolean.md),
[Integer](reference/Integer.md), [Real](reference/Real.md),
[String](reference/String.md) and [Function](reference/Function.md)
classes. The following statements assign primitive values to variables.

    var a = null;
    var b = true;
    var c = 1;
    var d = 3.14 * 2;
    var e = "Hello world";

Please also note that primitive values are automatically converted in
ObjectTalk when required.

    var f = e + c;

In the expression above c gets converted to a String as the addition
operator is applied to e (a String object) which wants its
argument also as a string.

In ObjectTalk, The **null** object is the only instance of the abstract
[Object](reference/Object.md) class. It's a special object as it can't
do anything. It is however helpful as a true non-value.

[Booleans](reference/Boolean.md) in ObjectTalk can only hold two values:
true and false. The default global language context defines the variables
**true** and **false** as a convenience.

[Integers](reference/Integer.md) represent negative and positive whole
numbers. On most systems, this number is implemented as a 64-bit number
meaning that the extremes are -9,223,372,036,854,775,808 and
9,223,372,036,854,775,807 (inclusive).

[Reals](reference/Real.md) in ObjectTalk are double-precision floating
point numbers. On most systems, Reals are implemented using 8 bytes
and have a range of 1.7E +/- 308 (15 digits).

In ObjectTalk, [Strings](reference/String.md) are captured in double
quotes and can span multiple lines. Strings may contain UTF-8 characters
(like "€") or use JSON style encoding like "Most Europeans like the
\u00C4.\n".

[Functions](String/Function.md) in ObjectTalk are primitive values.
This means that we can pass a function around like any other primitive.
The following code might be a little too complex for this overview, so
we'll defer an explanation until we'll get the the Language Guide.
The point is that a function is a first class citizen in ObjectTalk.

    function test1(callback) {
        callback();
    }

    test2 = function() {
        print("Hello, world!");
    };

    test1(test2);

## Collections

ObjectTalk supports two types of [Collections](reference/Collections.md)
([Arrays](reference/Array.md) arrays and
[Dictionaries](reference/Dict.md))
that can be nested if required.

[Arrays](reference/Array.md) are sequences of objects that can be
constructed using square brackets ([]) or through the Array class
constructor. The Array class has many methods to manipulate the content
of an array.

    var array1 = [ 1, 2, "test", 7 + 4, a ];
    var array2 = Array(3.14, 7, 34, 1);

    array1.append("new value");
    var thirdValue = array1[3];

[Dictionaries](reference/Dict.md) contain indexed key/value pairs that
are constructed using curly brackets ({}) or through the Dict class
constructor. The Dict class also has many methods to manipulate the
content of a dictionary.

    var dict1 = [ "First Name": "John", "Last Name": "Doe", "Age": 34 };
    var dict2 = Dict("Name", "John Doe", "Address", "Unknown");

    dict1["Last Update"] = "1 Apr 2000";
    var name = dict2["Name"];

Both arrays and dictionaries follow JSON rules and the ObjectTalk
compiler can therefore ingest JSON without any trouble.
This should also make it easier for those who are already familiar
with JSON encoding.

## Control Flow

ObjectTalk uses **if** to make conditionals and uses **for in**,
**while** and **do while** to make loops. Parentheses around the
condition or loop variable are optional. Braces around the body are required.

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

## Functions

## Object and Classes

## Error handling

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
