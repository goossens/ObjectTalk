The Audio Engine is basically another version of node-based programming.
Each node (circuit) provides signal processing abilities and wires are used to
connect the circuits. From a use perspective, the interface is very similar to the
nodes mentioned above but the processing is optimized for audio as it needs to
be real-time. Our ears are very good at detecting small glitches and we perceive
them as unwanted clicks, scratches or noise.

Currently there are circuits available to create modular synthesizers and effects
but in the future this will grow to components for guitar amplifiers, percussion
instruments and audio mixing.

![Audio](img/audio.png#center)

A circuit represents a small function that transforms inputs into outputs,
optionally using additional parameters. In ObjectTalk, circuits are visually
represented by rounded rectangles displaying a title at the top, inputs displayed
on the left and outputs on the rights. Inputs and outputs are referred to as pins
that can be connected with wires and can carry only one signal type.

Each circuit really feels like an object instance and drawing connections
between them is akin to visual programming. Today, in ObjectTalk, the
IDE has an audio editor that allows you to setup circuits and connect them
into a larger circuit. These circuits are automatically run in realtime.
The examples folder contains signal processing chains and circuits that
are used to implement audio processing tools ore instruments.
ObjectTalk already has over 20 nodes and many will follow depending on my needs.

Users can draw connections between nodes (called wires) and the editor ensures
that output and input nodes have compatible signal types, that inputs have a maximum
of one connection and that the created graph does not include recursion (i.e
stays acyclic). As wires are created/moved/deleted or values altered, the editor
automatically reroutes the signals given the user a realtime programming
experience.
