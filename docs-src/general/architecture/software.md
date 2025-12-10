The diagram below depicts the subsystems included in the ObjectTalk ecosystem.
As mentioned above, the four boxes on the right are available on both ObjectTalk
configurations whereas the five boxes on the left are only available when the
host operating system offers GUI libraries.

![Software Architecture](img/software-architecture.png#center)

**Operating System Foundation**

ObjectTalk currently runs on MacOS, Linux and Windows. In general, the main development
platform is MacOS and typically the latest version is used.
For Linux, Ubuntu 24.04 is used and on Billy boxes, Windows 11. The Github continuous
integration tasks also use the latest versions of these operating systems. For those
that use other Linux distributions, it is likely that ObjectTalk will compile out-of-the-box,
but this is not verified.

The entire ObjectTalk ecosystem is written in C++17 which is the most recent version that
is best supported on all platforms. Once C++20 finds better support, we will switch to it.

To simplify the ObjectTalk code baseline, the C++ standard library is used to the maximum
extend possible. Basic I/O function, filesystem access, containers and memory management
are simply used as is. Compiler developers typically do a good job optimizing things on each platform.

I know there are lot of people out there that believe they can do better than the standard library
(and they might be right) but the time required to optimize on multiple platforms is not worth it.
I’m old enough to have written operating systems in assembler and had to maintain C libraries
for even the most simple memory management or data structures. These days, I gladly take free
cross platform solutions and sacrifice a few CPU cycles (I have more of those than brain cells).

To prove that I'm a hypocrite, the one exception to the rule is shared pointers. The version
included in the standard library is thread safe and therefore slow. Given that ObjectTalk always
runs in a single thread, a replacement shared pointer class is included that is not thread safe but
makes ObjectTalk twice as fast. Off course I argue that I would have used the standard library
if it only had included a fast non-thread safe smart pointer.

**Language Engine**

The language engine uses some simple components to implement ObjectTalk. A lexical scanner turns
source code into tokens, the single-pass compiler translates these tokens to bytecode for a
platform-independent imaginary CPU, an optimizer turns simple code sequences into super instructions
and the stack-based virtual machine executes this bytecode.

![Language Architecture](img/language-architecture.png#center)

**Core Classes**

Given that ObjectTalk’s virtual machine only understands a stack of opaque objects, there members,
member functions and basic branching, even the most fundamental operations like additions,
string manipulation and I/O must be implemented in ObjectTalk classes. The core classes subsystem
provides fundamental classes for primitives (booleans, integers, reals, strings and functions),
collections (arrays, dictionaries and sets) as well as a set of operating system, filesystem and
stream classes.

**Network Framework**

These classes provide asynchronous access to network functions.
When implementing servers or I/O heavy applications, you typically have two options. Firstly,
you can go multithreaded and use many threads and thread pools to handle I/O request.
This approach however has proven not to be too scalable and most web servers today don’t use it
or have complicated strategies to deal with performance and bottlenecks. The other approach is
to use a single thread with an event loop that runs as fast as possible and simply responds to
operating system events whenever they happen. ObjectTalk uses this last approach based on the
[Libuv](http://docs.libuv.org/) library that offers asynchronous operations as a cross-platform
solutions. Libuv was originally developed for [Node.js](https://nodejs.org/) and is still in use
there. This means that it is well tested and maintained.

**Network Classes**

ObjectTalk provides a number of classes that make it easy to implement a web server on top of
the network framework. For those familiar with [Node.js](https://nodejs.org/) and the
[Express package](https://expressjs.com), you will recognize the similarities. If you need an
operational asynchronous web server, use [Node.js](https://nodejs.org/). If you want to use a
simple server using a few lines of code, this ObjectTalk library might be the ticket.

**Graphics Framework**

As mentioned above, the graphics framework is optional and will only be compiled in when the
host operating system provided the required GUI libraries. The foundation of this framework
is provided by a number of open source cross-platform libraries. [SDL3](https://github.com/libsdl-org/SDL)
is used as an system independent abstraction for windows and user/system interactions.
[BGFX](https://github.com/bkaradzic/bgfx) is used as a system independent abstraction to render
graphics supporting Metal, OpenGL, Vulkan and DirectX APIs. [Dear ImGui](https://github.com/ocornut/imgui)
is used as a neutral user interface library and all widgets are implemented with it.
The framework glues all of this together and ObjectTalk users will never have to deal with this level of detail.

In the original design for the graphics framework, the ObjectTalk language was used to construct all
the components for a user interface. Unfortunately, this led to very verbose code and it would take
pages of code to create all the things you would need for a user interface, 2D board or 3D scene.
In fact, the ratio between construction code and functionality was so bad that a new approach was implemented.

Today, the framework uses a variety of technics. A purely ObjectTalk driven approach is still
available to implement simple GUIs or 2D games. For 3D scenes, an Entity Component System (ECS)
as an architectural pattern to represent the elements of the scene. The IDE provides a visual
editor for ECS data which is easy to use. To make 3D scenes scriptable, a basic ObjectTalk scripting
component is available.

**Graphics Classes**

These classes provide scripting access to the graphics framework as well components to build GUIs.
ObjectTalk has a rich set of platform independent widgets that can be used to create user interfaces and apps.
In the included examples, the gui, arcade and pacman folders contain sample scripts to showcase
the power of these classes.

**Node Engine**

Many years ago, I developed a graphical node-based editor to configure computer I/O systems complete
with stream processing and protocol conversion. This worked really well and it allowed non-programmers
to assist during development, maintenance and operations. Once I played with Blender's nodes for geometry
and shaders, I thought it was a good idea to also add nodes to ObjectTalk. Please keep in mind though
that becoming a Blender clone is not one of the ambitions of this project.

Each node really feels like an object instance and drawing connections between them is akin to visual
programming. Today, in ObjectTalk, the IDE has an node editor that allows you to setup nodes and connect
them into a graph. These graphs are automatically reevaluated when values change. The examples folder
contains image processing chains and nodes are used in the scene examples to process images or create geometry.
ObjectTalk already has over 90 nodes already and many will follow depending on my needs.

**3D Scenes**

As mentioned above, 3D scenes are based on an Entity Component System (ECS) that allows you to define
entities (objects in the real world) and associate a variety of components with them that offer functionality.

The 3D scene ECS also has nodes and script components to make the whole thing more dynamic. In the
examples folder, you'll find automatically spinning cubes based on timers and an earth rotating based
on mouse movements. Both of these a driven by script components. The jungle and island examples use
nodes to procedurally create terrain and asset instances.

To round it all out, modern engine features like geometry/model importing, support for animated models,
deferred rendering, Physically Based Rendering (PBR), particle systems, Image Based Lighting (IBL),
Cascaded Shadow Maps (CSMs) and post processing with Fog, Bloom, God Rays, Fast Approximate Anti-Aliasing (FXAA),
Exposure/Contrast correction and Tone Mapping are also available.

**Integrated Development Environment (IDE)**

To top it all off, ObjectTalk comes with its own IDE to edit ObjectTalk scripts, scenes and nodes.
The IDE runs in a single window (with tabs) and has full undo/redo/cut/copy/paste capabilities
for all content types. From the IDE, script, scenes and nodes can be validated and run.
Errors are highlighted inline making it easy and fast to finish a project. It includes the
following editors:

- **ObjectTalk script editor**
	- This editor provides a powerful syntax highlighting capability modeled after a subset of Visual Studio Code.
	- The editor has full undo capabilities and works with UTF-8 encoded text.
	- The editor can launch ObjectTalk scripts and has a console for output and highlighting for errors.
	- A visual debugger is also available.

- **Node editor**
	- This editor allows the creation of node graphs.
	- The editor has full undo capabilities.
	- The editor automatically evaluates graphs so it can be used for realtime visual programming (e.g. image processing or asset generation).

- **Scene editor**
	- This editor allows the creation and configuration of entities and components.
	- Entities are stored in trees making it easy to create entity hierarchies.
	- The editor has full undo capabilities.
	- Scenes can be run directly from the IDE to ensures node and script components function properly.

- **Miscellaneous editors**
	- The IDE is also capable of editing pure text, JSON and Markdown files (complete with syntax highlighting).
