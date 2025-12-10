In ObjectTalk, nodes can support different data flows. Most nodes
process "fixed" flows that act as simple functions taking the provided
inputs (with optional node parameters), apply a one time algorithm
and produce outputs.

Some nodes (e.g. Instances on Faces) run the algorithm on each element
of the input (e.g. faces in a geometry). Those nodes create a context
and evaluate each input once per element. If a downstream input node is
a "varying" node, the context is accessed and the correct value provided. This
allows processing of composite data (e.g. geometries) by evaluating each
element (e.g. vertex or face).

Some nodes are chameleons (e.g. math nodes) and they can operate on
"fixed" and "varying" flows. The nodes are considered "flexible".
The node reference documentation highlights whether a node is considered
"fixed", "varying" or "flexible".

Input and output pins visualize their mode by drawing a circle in "fixed"
mode and a diamond in "varying" mode . Link are drawn with a solid line
for "fixed" connections and with dashed lines for "varying" connections.

![screenshot](img/data-flows.png#center)
