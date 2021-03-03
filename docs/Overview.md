# ObjectTalk Overview

Tradition suggests that the first program in a new language should print
the words “Hello, world!” on the screen. In ObjectTalk, this can be done
in a single line:

    print("Hello, world!")

If you have written code in C or any of the derived languages before, this
syntax might look familiar to you. In ObjectTalk, this line of code is a
complete program. You don’t need to import a separate library for
functionality like input/output or string handling. Code written at global
scope is used as the entry point for the program, so you don’t even need
a main() function. You don’t even need to write semicolons at the end of
every statement.

So where do we go from here. Well, we hope this overview gives you enough
information to start writing code in ObjectTalk by showing you how to
accomplish a variety of programming tasks. Don’t worry if you don’t
understand something, everything introduced in this overview is explained
in detail in the rest of the documentation.

## Variables

In ObjectTalk, variables do not have to be declared as they are created
in the appropriate scope through assignment. ObjectTalk makes no
distinction between constants and variables. If you set it and don't
change it, it's a constant. If you do change it, it's a variable.

## Primitive Values

ObjectTalk supports 5 primitive values that are derived from the
Boolean, Integer, Real, String and Function classes. The following
statements assign primitive values to a variable.

    a = true
    b = 1
    c = 3.14 * 2
    d = "Hello world"

Please also note that primitive values are automatically converted in
ObjectTalk when required.

    e = d + b

In the expression above b gets converted to a String as the addition
operator is applied to a String object that wants its argument to be
a string.

Strings in ObjectTalk are captured in single or double quotes and can
span multiple lines. Strings may contain UTF-8 characters (like "€") or
use JSON style encoding (like "Most Europeans like the \u00C4.\n").

In ObjectTalk, a function is a primitive value. This means that
we can pass a function around like any other primitive. The following
code might be a little too complex for this overview, so we'll defer an
explanation until we'll get the the Language Guide. The point is
that a function is a first class citizen in ObjectTalk.

    function test1(callback) {
        callback()
    }

    test2 = function() {
        print("Hello, world!")
    }

    test1(test2)

## Collections

ObjectTalk supports two types of collections (arrays and dictionaries)
that can be nested if required.

Arrays are sequences of objects that can be constructed using square
brackets ([]) or through the Array class constructor. The Array class
has many methods to manipulate the content of an array.

    array1 = [ 1, 2, "test", 7 + 4, a ]
    array2 = Array(3.14, 7, 34, 1)

	array1.append("new value")
	thirdValue = array1[3]

Dictionaries contain indexed key/value pairs that are constructed using
curly brackets ({}) or through the Dict class constructor. The Dict class
also has many methods to manipulate the content of a dictionary.

	dict1 = [ "First Name": "John", "Last Name": "Doe", "Age": 34 }
	dict2 = Dict("Name", "John Doe", "Address", "Unknown")

	dict1["Last Update"] = "1 Apr 2000"
	name = dict2["Name"]

Both arrays and dictionaries follow JSON rules and the ObjectTalk
compiler can therefore ingest properly formatted JSON without any trouble.
This should also make it easier for those who are already familiar
with JSON encoding.

## Control Flow

ObjectTalk uses **if** and **switch** to make conditionals and use
**for in**, **while** and **do while** to make loops.
Parentheses around the condition or loop variable are optional. Braces around the body are required.

    individualScores = [75, 43, 103, 87, 12]
    teamScore = 0

    foreach score in individualScores {
        if score > 50 {
            teamScore += 3

        } else {
            teamScore += 1
        }
    }

    print(teamScore)
    // Prints "11"

## Functions
