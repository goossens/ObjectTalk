An operator is a special symbol or phrase that you use to check, change,
or combine values. For example, the addition operator (**+**) adds two
objects, as in **var i = 1 + 2**, and the logical AND operator (**&&**)
combines two Boolean values, as in
**if enteredDoorCode && passedRetinaScan**.
ObjectTalk supports the operators you may already know from languages
like C.

**Terminology**

Operators are unary, binary, or ternary:

* **Unary operators** operate on a single target (such as -a). Unary
prefix operators appear immediately before their target (such as !b), and
unary postfix operators appear immediately after their target (such as c++).
* **Binary operators** operate on two targets (such as 2 + 3) and are infix
because they appear in between their two targets.
* **Ternary operators** operate on three targets. Like C, ObjectTalk
has only one ternary operator, the ternary conditional operator (a ? b : c).

The values that operators affect are operands. In the expression 1 + 2, the + symbol is a binary operator and its two operands are the values 1 and 2.

**Assignment Operator**

The assignment operator **(a = b)** initializes or updates the value of a
with the value of b:

	var b = 10;
	var a = 5;
	a = b;
	// a is now equal to 10

**Arithmetic Operators**

ObjectTalk supports the four standard arithmetic operators:

* Addition (+)
* Subtraction (-)
* Multiplication (*)
* Division (/)

Examples:

	1 + 2       // equals 3
	5 - 3       // equals 2
	2 * 3       // equals 6
	10.0 / 2.5  // equals 4.0

The addition operator is also supported for String concatenation or for
any class that implements the \_\_add\_\_ member function.

	"hello, " + "world"  // equals "hello, world"

**Remainder Operator**

The remainder operator **(a % b)** works out how many multiples of b will
fit inside a and returns the value that’s left over (known as the
remainder).

**Unary Minus Operator**

The sign of a numeric value can be toggled using a prefixed -, known as
the unary minus operator:

	var three = 3;
	var minusThree = -three;       // equals -3
	var plusThree = -minusThree;   // equals 3, or "minus minus three"

The unary minus operator (-) is prepended directly before the value it
operates on, without any white space.

**Unary Plus Operator**

The unary plus operator **(+)** simply returns the value it operates on,
without any change:

	let minusSix = -6;
	let alsoMinusSix = +minusSix;  // equals -6

Although the unary plus operator doesn’t actually do anything, you can
use it to provide symmetry in your code for positive numbers when also
using the unary minus operator for negative numbers.

**Compound Assignment Operators**

Like C, ObjectTalk provides compound assignment operators that combine
assignment **(=)** with another operation. One example is the addition
assignment operator **(+=)**:

	var a = 1;
	a += 2;
	// a is now equal to 3

The expression a += 2 is shorthand for a = a + 2. Effectively, the
addition and the assignment are combined into one operator that performs
both tasks at the same time. The generated code for a += 2 however is
identical to the code generated for a = a + 2. a += 2 is therefore just
"syntactical sugar".

**Comparison Operators**

ObjectTalk supports the following comparison operators:

* Equal to (a == b)
* Not equal to (a != b)
* Greater than (a > b)
* Less than (a < b)
* Greater than or equal to (a >= b)
* Less than or equal to (a <= b)

Examples:

	1 == 1   // true because 1 is equal to 1
	2 != 1   // true because 2 isn't equal to 1
	2 > 1    // true because 2 is greater than 1
	1 < 2    // true because 1 is less than 2
	1 >= 1   // true because 1 is greater than or equal to 1
	2 <= 1   // false because 2 isn't less than or equal to 1

Comparison operators are often used in conditional statements, such as
the if statement:

	var name = "world";

	if name == "world" {
	    print("hello, world")

	} else {
	    print("I'm sorry ", name, ", but I don't recognize you")
	}

	// prints "hello, world"

**Ternary Conditional Operator**

The ternary conditional operator is a special operator with three parts,
which takes the form question ? answer1 : answer2. It’s a shortcut for
evaluating one of two expressions based on whether question is true or
false. If question is true, it evaluates answer1 and returns its value;
otherwise, it evaluates answer2 and returns its value. The ternary
conditional operator is shorthand for the code below:

	if question {
	    answer1

	} else {
	    answer2
	}

Here’s an example, which calculates the height for a table row. The row
height should be 50 points taller than the content height if the row has a
header, and 20 points taller if the row doesn’t have a header:

	var contentHeight = 40;
	var hasHeader = true;
	var rowHeight = contentHeight + (hasHeader ? 50 : 20);
	// rowHeight is equal to 90

The example above is shorthand for the code below:

	var contentHeight = 40;
	var hasHeader = true;
	var rowHeight;

	if hasHeader {
	    rowHeight = contentHeight + 50;

	} else {
	    rowHeight = contentHeight + 20;
	}

	// rowHeight is equal to 90

The first example’s use of the ternary conditional operator means that
rowHeight can be set to the correct value on a single line of code, which
is more concise than the code used in the second example.

The ternary conditional operator provides an efficient shorthand for
deciding which of two expressions to consider. Use the ternary conditional
operator with care, however. Its conciseness can lead to hard-to-read code
if overused. Avoid combining multiple instances of the ternary conditional
operator into one compound statement.

**Logical Operators**

Logical operators modify or combine the Boolean logic values true and
false. ObjectTalk supports the three standard logical operators found in
C-based languages:

* Logical NOT (!a)
* Logical AND (a && b)
* Logical OR (a || b)

**Logical NOT Operator**

The logical NOT operator **(!a)** inverts a Boolean value so that true
becomes false, and false becomes true.

The logical NOT operator is a prefix operator, and appears immediately
before the value it operates on, without any white space. It can be read
as “not a”, as seen in the following example:

	var allowedEntry = false;

	if !allowedEntry {
	    print("ACCESS DENIED");
	}

	// Prints "ACCESS DENIED"

The phrase if !allowedEntry can be read as “if not allowed entry.” The
subsequent line is only executed if “not allowed entry” is true; that is,
if allowedEntry is false.

As in this example, careful choice of Boolean constant and variable names
can help to keep code readable and concise, while avoiding double negatives
or confusing logic statements.

**Logical AND Operator**

The logical AND operator **(a && b)** creates logical expressions where
both values must be true for the overall expression to also be true.

If either value is false, the overall expression will also be false. In
fact, if the first value is false, the second value won’t even be
evaluated, because it can’t possibly make the overall expression equate to
true. This is known as short-circuit evaluation.

This example considers two Bool values and only allows access if both
values are true:

	var enteredDoorCode = true;
	var passedRetinaScan = false;

	if enteredDoorCode && passedRetinaScan {
	    print("Welcome!");

	} else {
	    print("ACCESS DENIED");
	}

	// Prints "ACCESS DENIED"

**Logical OR Operator**

The logical OR operator **(a || b)** is an infix operator made from two
adjacent pipe characters. You use it to create logical expressions in which
only one of the two values has to be true for the overall expression to be
true.

Like the Logical AND operator above, the Logical OR operator uses short
circuit evaluation to consider its expressions. If the left side of a
Logical OR expression is true, the right side isn’t evaluated, because it
can’t change the outcome of the overall expression.

In the example below, the first Bool value (hasDoorKey) is false, but the
second value (knowsOverridePassword) is true. Because one value is true,
the overall expression also evaluates to true, and access is allowed:

	var hasDoorKey = false;
	var knowsOverridePassword = true;

	if hasDoorKey || knowsOverridePassword {
	    print("Welcome!");

	} else {
	    print("ACCESS DENIED");
	}

	// Prints "Welcome!"

**Combining Logical Operators**

You can combine multiple logical operators to create longer compound expressions:

	if enteredDoorCode && passedRetinaScan || hasDoorKey || knowsOverridePassword {
	    print("Welcome!");

	} else {
	    print("ACCESS DENIED");
	}

	// Prints "Welcome!"

This example uses multiple && and || operators to create a longer compound
expression. However, the && and || operators still operate on only two
values, so this is actually three smaller expressions chained together. The
example can be read as:

If we’ve entered the correct door code and passed the retina scan, or if we
have a valid door key, or if we know the emergency override password, then
allow access.

Based on the values of enteredDoorCode, passedRetinaScan, and hasDoorKey,
the first two subexpressions are false. However, the emergency override
password is known, so the overall compound expression still evaluates to
true.

**Explicit Parentheses**

It’s sometimes useful to include parentheses when they’re not strictly
needed, to make the intention of a complex expression easier to read. In
the door access example above, it’s useful to add parentheses around the
first part of the compound expression to make its intent explicit:

	if (enteredDoorCode && passedRetinaScan) || hasDoorKey || knowsOverridePassword {
	    print("Welcome!");

	} else {
	    print("ACCESS DENIED");
	}

	// Prints "Welcome!"

The parentheses make it clear that the first two values are considered as
part of a separate possible state in the overall logic. The output of the
compound expression doesn’t change, but the overall intention is clearer to
the reader. Readability is always preferred over brevity; use parentheses
where they help to make your intentions clear.

**Operator Overview and Priorities**

The table below list all the operators used in ObjectTalk including their
priority and member function name.

| Operator | Description | Priority | Member Function |
| -------- | ----------- | -------- | --------------- |
| [] | Index | 1 | \_\_index\_\_ |
| () | Call | 1 | \_\_call\_\_ |
| . | Member | 1 | \_\_member\_\_ |
| ++ | Post-increment | 1 | \_\_inc\_\_ |
| -- | Post-decrement  | 1 | \_\_dec\_\_ |
| - | Unary minus | 2 | \_\_neg\_\_ |
| + | Unary plus | 2 | \_\_plus\_\_ |
| ! | Logical NOT | 2 | \_\_not\_\_ |
| ~ | Bitwise NOT | 2 | \_\_bnot\_\_ |
| ++ | Pre-increment | 2 | \_\_inc\_\_ |
| -- | Pre-decrement | 2 | \_\_dec\_\_ |
| * | Multiply | 3 | \_\_mul\_\_ |
| / | Divide | 3 | \_\_div\_\_ |
| ** | Power | 3 | \_\_power\_\_ |
| % | Remainder | 3 | \_\_mod\_\_ |
| + | Add | 4 | \_\_add\_\_ |
| - | Subtract | 4 | \_\_sub\_\_ |
| << | Bitwise left shift | 5 | \_\_lshift\_\_ |
| >> | Bitwise right shift | 5 | \_\_rshift\_\_ |
| < | Less than | 6 | \_\_lt\_\_ |
| <= | Less than or equal | 6 | \_\_le\_\_ |
| > | Greater than | 6 | \_\_gt\_\_ |
| >= | Greater than or equal | 6 | \_\_ge\_\_ |
| in | is in | 6 | \_\_contains\_\_ |
| not in | is not in | 6 | |
| == | Equal | 7 | \_\_eq\_\_ |
| != | Not equal | 7 | \_\_ne\_\_ |
| & | Bitwise AND | 8 | \_\_band\_\_ |
| ^ | Bitwise XOR | 9 | \_\_bxor\_\_ |
| \| | Bitwise OR | 10 | \_\_bor\_\_ |
| && | Logical AND | 11 | \_\_and\_\_ |
| \|\| | Logical OR | 12 | \_\_or\_\_ |
| ?: | Ternary conditional | 13 | |
| = | Assign| 14 | \_\_assign\_\_ |
| *= | Multiply and assign | 14 | |
| /= | Divide and assign | 14 | |
| %= | Remainder and assign | 14 | |
| += | Add and assign | 14 | |
| -= | Subtract and assign | 14 | |
| <<= | Left bit shift and assign | 14 |  |
| >>= | Right bit shift and assign | 14 |  |
| &= | Bitwise AND and assign | 14 | |
| \|= | Bitwise OR and assign | 14 | |
| ^= | Bitwise XOR and assign | 14 | |
