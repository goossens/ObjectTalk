As was explained above, the ObjectTalk ecosystem comes as a single executable file called “ot”.
This single executable can run in four different modes depending on how it was launched.

**Simple Mode**

![Simple Mode](img/simple-mode.png)

In its simplest form, “ot” is a scripting engine that can execute ObjectTalk scripts from the command line.
In this mode, the main (and only) thread compiles and executes the code until the script ends.

**Server Mode**

![Server Mode](img/server-mode.png)

In server mode, a script is started like described above in Simple Mode. The difference is that at
some point a server script starts the event loop after which control passed to the engine.
Only callbacks are made from that point forward similar to other asynchronous frameworks like Node.js.
The startup script as well as the event loop are all executed from the main and only thread.

**Graphics Mode**

![Graphics Mode](img/graphics-mode.png)

In graphics mode, the starting point can be a script or a JSON file that contains all the information
detailing the structure and capabilities of a GUI, scene or node-based processing.
The JSON file can refer to external assets such as images, sprites or models and can also refer to
ObjectTalk scripts that act as controllers in the created graphical world.

The required JSON file can be constructed by hand but this is not recommended. The provided IDE
has dedicated editors for all the graphical Entity Component System (ECS) and node-based JSON files.

In graphical mode a number of threads are used to run the application which might be slightly
different based on host operating system. The main thread will be used as the windowing and
rendering thread. This thread interacts with the native windows system and communicates with the GPU.
it is also used to run the application, do most of the CPU work and run the event loop.
The ObjectTalk callbacks will also run in this thread.

Additional worker threads might be spawned to do things like asset loading or heavier
computations.

**Integrate Development Environment (IDE) Mode**

![IDE Mode](img/ide-mode.png)

In the Integrate Development Environment (IDE) mode is the IDE will act as the main application
just like any other program in graphics mode. When the IDE runs a script or graphics application,
a subprocess is spawned in the applicable mode. Bi-directional Inter Process Communication (IPC) is
setup to allow the IDE to communicate with the ObjectTalk script, GUI, scene or
node-based apps. This is how the visual debugger is implemented.

**Rendering Pipeline**

![IDE Mode](img/rendering-pipeline.png#center)

ObjectTalk has a significant number of GUI capabilities and it therefore also has a complex rendering
pipeline. At the heart of this pipeline is the master compositor that brings everything together
before it can be shown on the screen. In ObjectTalk, the Dear ImGui library is used for this purpose
(see step 7 in the diagram above). This might not be the fastest approach but it sure is the most
convenient approach. Dear ImGui provides a great immediate mode rendering philosophy and it comes
with a rich set of widgets. ObjectTalk adds a lot of widgets to this (either from external projects
or included in ObjectTalk).

All the complexity of rendering a 3D scene is depicted above in the diagram as steps 1 through 6.
At the end, the rendered scene is in a GPU texture and can simply be rendered to the screen through
a Dear ImGui image widget.

Below is a description of the 6 steps to create the final 3D scene texture:

1. If a scene generates shadows, Cascaded Shadow Maps (CSM) are created in the first steps so
they can be used in follow-on lighting calculations.
1. A simple background pass clears the scene composite buffer and sets it to a desired color.
1. Opaque geometries, models, terrain and grass goes through a
[Deferred shading](https://en.wikipedia.org/wiki/Deferred_shading) process that first renders
all entities to a G-buffer and then applies lighting as a second pass.
1. Transparent entities are rendered directly to the composite buffer to ensure transparency.
1. The composite buffer is send through post processing to apply desired effects.
