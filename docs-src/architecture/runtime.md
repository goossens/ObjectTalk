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
detailing the structure and capabilities of a board, scene or node-based processing.
The JSON file can refer to external assets such as images, sprites or models and can also refer to
ObjectTalk scripts that act as controllers in the created graphical world.

The required JSON file can be constructed by hand but this is not recommended. The provided IDE
has dedicated editors for all the graphical Entity Component System (ECS) JSON files.

In graphical mode a number of threads are used to run the application which might be slightly
different based on host operating system. The main thread will be used as the windowing and
rendering thread. This thread interacts with the native windows system and communicates with the GPU.
It might be more intuitive to make this a secondary thread but most operating systems require
that these tasks are execute from the primary or startup thread.

A second thread will therefore be used to run the application, do most of the CPU work and run
the event loop. The ObjectTalk callbacks will also run in this thread.

The graphical framework automatically and transparently connects the main (graphical) thread
with the application or scripts thread.

**Integrate Development Environment (IDE) Mode**

![IDE Mode](img/ide-mode.png)

In the Integrate Development Environment (IDE) mode is the IDE will act as the main application
just like any other program in graphics mode. When the IDE runs a script or graphics application,
a subprocess is spawned in the applicable mode. Bi-directional Inter Process Communication (IPC) is
setup to allow the IDE to communicate with the ObjectTalk script, GUI, board, scene or
node-based apps.
