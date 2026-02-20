![screenshot](img/circuit-anatomy.png#right)

A circuit is depicted as a rounded rectangle with a title bar at the top,
optional inputs on the left, optional outputs on the right and
optional parameters in the middle of the rectangle.

**Title Bar**

At the top of a circuit is a title bar which by default contains
the name of the circuit. By double clicking on the title, users can
name a circuit anything they want to better express its function
in the signal flow. The background of the title bar is color coded to
represent the category of the circuit (see Circuit Categories below).

**Input Pins**

Optional inputs to a circuit are represented on the left side of that circuit
by what's called pins. Each pin represents an input of a specified type
that is color coded (see Pin Types below). Input pins can only receive
a signal from one output pin. and the pin data flows have to match (which
is enforced by the IDE).

**Output Pins**

Optional output from a circuit are represented on the right side of the
circuit by pins. Each pin represents an output with a specified signal type
that is color coded (see pin types below). Output pins can be connected
to multiple input pins as long as the signal types match (also enforced
by th IDE).

**Circuit Parameters**

Some circuits also might have additional parameters or visualizations
to help the circuit perform its function. The circuit parameters are
rendered in the middle of the circuit.
