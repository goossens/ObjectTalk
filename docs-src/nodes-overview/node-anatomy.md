![screenshot](img/node-anatomy.png#right)

A node is depicted as a rounded rectangle with a title bar at the top,
optional inputs on the left, optional outputs on the right and
optional parameters in the middle of the rectangle.

**Title Bar**

At the top of the node is a title bar which by default contains
the name of the node. By double clicking on the title, users can
name a node anything they want to better express its function
in a graph. The background of the title bar is color coded to
represent the category of the node (see Node Categories below).

**Input Pins**

Optional inputs to a node are represented on the left side of that node by
what's called pins. Each pin represents an input with a specified type
that is color coded (see Pin Types below). Input pins can only receive
data from one output pin. and the pin data types have to match (which
is enforced by the IDE).

If an input pin is not connected and the data types allows easy
specification by the user, a field will be visible to the right
of the pin's name to allow the user to specify a static value.

**Output Pins**

Optional output from a node are represented on the right side of the node
by pins. Each pin represents an output with a specified type
that is color coded (see pin types below). Output pins can be connected
to multiple input pins as long as the data types match (also enforced
by th IDE).

**Node Parameters**

Some nodes also might have additional parameters or visualizations
to help the node perform its function. The node parameters are
rendered in the middle of the node.
