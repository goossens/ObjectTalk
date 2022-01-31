[![MacOS status](https://github.com/goossens/ObjectTalk/actions/workflows/macos.yml/badge.svg)](https://github.com/goossens/ObjectTalk/actions)
[![Ubuntu status](https://github.com/goossens/ObjectTalk/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/goossens/ObjectTalk/actions)
[![Windows status](https://github.com/goossens/ObjectTalk/actions/workflows/windows.yml/badge.svg)](https://github.com/goossens/ObjectTalk/actions)

[![Repo size](https://img.shields.io/github/repo-size/goossens/ObjectTalk)](https://github.com/goossens/ObjectTalk/actions)
[![Lines of code](https://img.shields.io/tokei/lines/github/goossens/ObjectTalk)](https://github.com/goossens/ObjectTalk/actions)
[![Repo activity](https://img.shields.io/github/commit-activity/w/goossens/ObjectTalk)](https://github.com/goossens/ObjectTalk/actions)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Maintained](https://img.shields.io/maintenance/yes/2022)
![Version](https://img.shields.io/badge/version-0.2-blue)

# ObjectTalk Scripting Language

![Logo](docs/img/logo-small.png)

Welcome to the ObjectTalk scripting language. ObjectTalk is a simple
object-oriented scripting language that has evolved over decades and
was heavily influenced by other languages. As a long time IT
professional, I've seen a lot of software development with all its
greatness and flaws. ObjectTalk represents what I think is a
well-balanced scripting language that is easy to learn and can make
even a novice look good quickly. If you are interested in how I got
to where we are today, please see the
[history](https://goossens.github.io/ObjectTalk/tour.html#history)
page for the story behind ObjectTalk.

So before continuing, let me address the elephant in the room. Why the
heck would the world need another scripting language? Well, the simple
answer is: it doesn't as we already have more than enough of them! I
wouldn't even suggest ObjectTalk is a language for serious software
development. If you need to deploy code, look elsewhere. ObjectTalk is
however is a good starting point to learn programming and it's also
good if you want to learn language development. Those of you who have
ever looked at the code for compilers and interpreters of populars
languages will probably know what I mean.

Full documentation for ObjectTalk can be found in
[the manual](https://goossens.github.io/ObjectTalk/).

## Building and Installing ObjectTalk

Instructions for building and installing ObjectTalk on various
platforms can be found in the
[installation manual](https://goossens.github.io/ObjectTalk/installation.html).
Currently, instructions are available for MacOS, Linux and Windows.

## Special Thanks

This project uses Open Source products without which it would have
been a lot harder to implement ObjectTalk. Special thanks goes to
the following communities:

* [CMake](https://cmake.org) - cross-platform family of tools designed to build, test and package software.
* [libuv](https://libuv.org) - multi-platform support library with a focus on asynchronous I/O.
* [argparse](https://github.com/p-ranav/argparse) - command line argument parser.
* [llhttp](https://llhttp.org) - HTTP protocol parser.
* [multipart-parser](https://github.com/francoiscolas/multipart-parser) - HTTP multipart body parser.

In case the GUI module is included, we also use:

* [GLFW](https://www.glfw.org) - multi-platform library for desktop application development.
* [bgfx](https://github.com/bkaradzic/bgfx) - cross-platform, graphics API agnostic rendering library.
* [Dear ImGui](https://github.com/ocornut/imgui) - graphical user interface library.
* [glm](https://github.com/g-truc/glm) - mathematics library for graphics software.
* [Box2D](https://box2d.org) - 2D physics engine.
* [tweeny](https://github.com/mobius3/tweeny) - animation library.
* [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog) - file selection dialog for Dear ImGui.
* [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit) - syntax highlighting text editor for Dear ImGui.

A special thanks also goes to
[Gunther Rademacher](https://github.com/GuntherRademacher) for his
["RailRoad Diagram Generator"](https://www.bottlecaps.de/rr/ui) which
is used in the documentation to visualize the ObjectTalk grammar.

A another special thanks goes to the team that wrote the
[Swift](https://swift.org) computer language
[documentation](https://swift.org/documentation/). It is awesome and
I've respectfully reused a lot of it to document ObjectTalk.

A final special thanks goes to the team that wrote the
[Tutorials](https://threejsfundamentals.org) for
[Three.js](https://threejs.org). The 3D ObjectTalk tutorial is based on it.

More (smaller) credits can be found in the GUI module:

* [Algorithms](gui/README.md)
* [Assets](examples/3d/assets/README.md)
* [Fonts](gui/utilities/README.md)

## License

Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, see <https://opensource.org/licenses/MIT>.
