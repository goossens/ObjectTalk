Many years ago, I developed a graphical node-based editor to configure
computer I/O systems complete with stream processing and protocol
conversion. This worked really well and it allowed non-programmers
to assist during development, maintenance and operations. Once I played
with Blender's nodes for geometry and shaders, I thought it was a good
idea to also add nodes to ObjectTalk. Please keep in mind though that
becoming a Blender clone is not one of the ambitions of this project.

Each node really feels like an object instance and drawing connections
between them is akin to visual programming. Today, in ObjectTalk, the
IDE has an node editor that allows you to setup nodes and connect them
into a graph. These graphs are automatically reevaluated when values
change. The examples folder contains image processing chains and nodes
are used in the scene examples to process images or create geometry.
ObjectTalk already has over 90 nodes already and many will follow
depending on my needs.

![Nodes](img/nodes.png#center)
