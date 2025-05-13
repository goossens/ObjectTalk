The ObjectTalk ecosystem was designed as a cross-platform capability. Today, ObjectTalk
runs on MacOS, Linux and Windows thanks to a large number of great open source packages.

The ObjectTalk ecosystem is contained in a single executable that embeds all modules and
their resources into one unit. Originally, the design called for a small executable that would
load shared modules/assets on demand. It was however decided not to continue with this
approach as dealing with DLL hell on Windows, keeping versions synchronized and putting
things all over the filesystem would lead to fragmentation and possibly missing/incompatible
pieces. Today, the ObjectTalk ecosystem compiles into that single unit that contains everything.
The file called “ot” can be moved around with easy and will always be complete, coherent and in sync.

During the compilation of ObjectTalk it is decided whether some user interface subsystems are
included. On headless Linux systems, the Graphical User Inteface (GUI) is omitted and
the “ot” executable can only run command line scripts or simple servers like a web server.
When the GUI libraries are available on the host system, “ot” is compiled to offer all subsystems.
On MacOS and Windows this is the default but even here a stripped-down version can be compiled
by setting a flag during compilation.

Github is used as the [source code repository](https://github.com/goossens/ObjectTalk) and on
every commit, the latest versions of the “ot” executable are built for MacOS, Linux and Windows
and can be downloaded. Stable versions will also be made available from the same Github repository.
Documentation for ObjectTalk is also included in the repository and the
[documentation website](https://goossens.github.io/ObjectTalk) is automatically updated on every
commit (off course the developer has to ensure the documentation is inline with the software.
Documentation still doesn’t write itself).
