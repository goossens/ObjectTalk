ObjectTalk provides a variety of control flow statements. These include
while loops to perform a task multiple times; if statements to execute
different branches of code based on certain conditions.
Swift also provides a for-in loop that makes it easy to iterate over
arrays, dictionaries, sets, strings, and other sequences.

**If**

In its simplest form, the **if** statement has a single condition.
It executes a set of statements only if that condition is true.

	var temperatureInFahrenheit = 30;

	if temperatureInFahrenheit <= 32 {
		print("It's very cold. Consider wearing a scarf.");
	}

	// Prints "It's very cold. Consider wearing a scarf."

The example above checks whether the temperature is less than or equal
to 32 degrees Fahrenheit (the freezing point of water). If it is, a
message is printed. Otherwise, no message is printed, and code execution
continues after the if statement’s closing brace.

The **if** statement can provide an alternative set of statements,
known as an **else** clause, for situations when the if condition is false.
These statements are indicated by the else keyword.

	var temperatureInFahrenheit = 40;

	if temperatureInFahrenheit <= 32 {
		print("It's very cold. Consider wearing a scarf.");

	} else {
		print("It's not that cold. Wear a t-shirt.");
	}

	// Prints "It's not that cold. Wear a t-shirt."

One of these two branches is always executed. Because the temperature has
increased to 40 degrees Fahrenheit, it’s no longer cold enough to advise
wearing a scarf and so the else branch is triggered instead.

You can chain multiple if statements together to consider additional
clauses by using the **elif** statement.

	var temperatureInFahrenheit = 90;

	if temperatureInFahrenheit <= 32 {
		print("It's very cold. Consider wearing a scarf.");

	} elif temperatureInFahrenheit >= 86 {
		print("It's really warm. Don't forget to wear sunscreen.");

	} else {
		print("It's not that cold. Wear a t-shirt.");
	}

	// Prints "It's really warm. Don't forget to wear sunscreen."

Here, an additional **elif** statement was added to respond to
particularly warm temperatures. The final else clause remains,
and it prints a response for any temperatures that are neither too warm
nor too cold.

The final else clause is optional, however, and can be excluded if the
set of conditions doesn’t need to be complete.

	var temperatureInFahrenheit = 72;

	if temperatureInFahrenheit <= 32 {
		print("It's very cold. Consider wearing a scarf.");

	} elif temperatureInFahrenheit >= 86 {
		print("It's really warm. Don't forget to wear sunscreen.");
	}

Because the temperature is neither too cold nor too warm to trigger the
if or else if conditions, no message is printed.

**While Loops**

A while loop performs a set of statements until a condition becomes false.
These kinds of loops are best used when the number of iterations isn’t
known before the first iteration begins. ObjectTalk provides two kinds of
while loops:

* **while** evaluates its condition at the start of each pass through the loop.
* **do-while** evaluates its condition at the end of each pass through the loop.

**While**

A while loop starts by evaluating a single condition. If the condition is
true, a set of statements is repeated until the condition becomes false.

Here’s the general form of a while loop:

	while condition {
		statements
	}

This example plays a simple game of Snakes and Ladders (also known as
Chutes and Ladders):

The rules of the game are as follows:

* The board has 25 squares, and the aim is to land on or beyond square 25.
* The player’s starting square is “square zero”, which is just off the bottom-left corner of the board.
* Each turn, you roll a six-sided dice and move by that number of squares, following the horizontal path indicated by the dotted arrow above.
* If your turn ends at the bottom of a ladder, you move up that ladder.
* If your turn ends at the head of a snake, you move down that snake.

The game board is represented by an array of integers. Its size is based
on a constant called finalSquare, which is used to initialize the array
and also to check for a win condition later in the example. Because the
players start off the board, on “square zero”, the board is initialized
with 26 zero Int values, not 25.

	var finalSquare = 25;

	var board = Array();
	board.fill(finalSquare + 1, 0);

Some squares are then set to have more specific values for the snakes and
ladders. Squares with a ladder base have a positive number to move you up
the board, whereas squares with a snake head have a negative number to move
you back down the board.

	board[03] = +08; board[06] = +11; board[09] = +09; board[10] = +02;
	board[14] = -10; board[19] = -11; board[22] = -02; board[24] = -08;

Square 3 contains the bottom of a ladder that moves you up to square 11.
To represent this, board[03] is equal to +08, which is equivalent to an
integer value of 8 (the difference between 3 and 11). To align the values
and statements, the unary plus operator (+i) is explicitly used with the
unary minus operator (-i) and numbers lower than 10 are padded with zeros.

	var square = 0;
	var diceRoll = 0;

	while square < finalSquare {
		// roll the dice
		diceRoll += 1;

		if diceRoll == 7 {
			diceRoll = 1;
		}

		// move by the rolled amount
		square += diceRoll;

		// if we're on the board, move up or down for a snake or ladder
		if square < board.count {
			square += board[square];
		}
	}

	print("Game over!");

The example above uses a very simple approach to dice rolling. Instead of
generating a random number, it starts with a diceRoll value of 0. Each time
through the while loop, diceRoll is incremented by one and is then checked
to see whether it has become too large. Whenever this return value equals
7, the dice roll has become too large and is reset to a value of 1. The
result is a sequence of diceRoll values that’s always 1, 2, 3, 4, 5, 6, 1,
2 and so on.

After rolling the dice, the player moves forward by diceRoll squares. It’s
possible that the dice roll may have moved the player beyond square 25, in
which case the game is over. To cope with this scenario, the code checks
that square is less than the board array’s count property. If square is
valid, the value stored in board[square] is added to the current square
value to move the player up or down any ladders or snakes.

The current while loop execution then ends, and the loop’s condition is
checked to see if the loop should be executed again. If the player has
moved on or beyond square number 25, the loop’s condition evaluates to
false and the game ends.

A while loop is appropriate in this case, because the length of the game
isn’t clear at the start of the while loop. Instead, the loop is executed
until a particular condition is satisfied.

**Do-While**

The other variation of the while loop, known as the **do-while** loop,
performs a single pass through the loop block first, before considering the
loop’s condition. It then continues to repeat the loop until the condition
is false.

Here’s the general form of a do-while loop:

	do {
		statements
	} while condition;

Here’s the Snakes and Ladders example again, written as a do-while loop
rather than a while loop. The values of finalSquare, board, square, and
diceRoll are initialized in exactly the same way as with a while loop.

	var finalSquare = 25;

	var board = Array();
	board.fill(finalSquare + 1, 0);

	board[03] = +08; board[06] = +11; board[09] = +09; board[10] = +02;
	board[14] = -10; board[19] = -11; board[22] = -02; board[24] = -08;

	var square = 0;
	var diceRoll = 0;

In this version of the game, the first action in the loop is to check for
a ladder or a snake. No ladder on the board takes the player straight to
square 25, and so it isn’t possible to win the game by moving up a ladder.
Therefore, it’s safe to check for a snake or a ladder as the first action
in the loop.

At the start of the game, the player is on “square zero”. board[0] always
equals 0 and has no effect.

	do {
		// move up or down for a snake or ladder
		square += board[square];

		// roll the dice
		diceRoll += 1;

		if diceRoll == 7 {
			diceRoll = 1;
		}

		// move by the rolled amount
		square += diceRoll;
	} while square < finalSquare;

	print("Game over!");

After the code checks for snakes and ladders, the dice is rolled and the
player is moved forward by diceRoll squares. The current loop execution
then ends.

The loop’s condition (while square < finalSquare) is the same as before,
but this time it’s not evaluated until the end of the first run through the
loop. The structure of the do-while loop is better suited to this game
than the while loop in the previous example. In the do-while loop
above, square += board[square] is always executed immediately after the
loop’s while condition confirms that square is still on the board. This
behavior removes the need for the array bounds check seen in the while loop
version of the game described earlier.

**For-In Loops**

You use the **for-in** loop to iterate over a sequence, such as items
in an array, dictionary, set or characters in a string.

This example uses a for-in loop to iterate over the items in an array:

	var names = ["Anna", "Alex", "Brian", "Jack"];

	for name in names {
		print("Hello, ", name, "!");
	}

	// Hello, Anna!
	// Hello, Alex!
	// Hello, Brian!
	// Hello, Jack!

To use a preset range, you can use the **range** generator:

	for number in range(4) {
		print(number);
	}

	// prints 1 2 3 4

	for number in range(2, 6) {
		print(number);
	}

	// prints 2 3 4 5 6

	for number in range(5, 9, 2) {
		print(number);
	}

	// prints 5 7 9

	for number in range(10, 6, -2) {
		print(number);
	}

	// prints 10 8 6
