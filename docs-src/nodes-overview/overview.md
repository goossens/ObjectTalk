Node-based programming is a visual approach to programming that uses
interconnected nodes to represent different actions and data flows.
These nodes are essentially "blocks" of code that perform specific tasks
and they are linked together to create
[Directed Acyclic Graph (DAG)](https://en.wikipedia.org/wiki/Directed_acyclic_graph),
where data flows from one node to another.

<div align="center">

![Nodes](img/nodes.png)

</div>

A node represents a small function that transforms inputs into outputs,
optionally using additional parameters. In ObjectTalk, nodes are visually represented
by rectangles displaying a title at the top, inputs displayed on the left and
outputs on the rights. Inputs and outputs are referred to as pins that can be connected
and can carry only one datatype.

Each node really feels like an object instance and drawing connections
between them is akin to visual programming. Today, in ObjectTalk, the
IDE has an node editor that allows you to setup nodes and connect them
into a graph. These graphs are automatically reevaluated when values
change. The examples folder contains image processing chains and nodes
are used in the scene examples to process images or create geometry.
ObjectTalk already has over 50 nodes already and many will follow
depending on my needs.

Users can draw connections between nodes (called links) and the editor ensures
that output and input nodes have compatible datatypes, that inputs have a maximum
of one connection and that the created graph does not include recursion (i.e
stays acyclic). As links are created/moved/deleted or values entered, the editor
automatically (re-)evaluates the graphs given the user a realtime programming
experience.

**Anatomy of a Node**

