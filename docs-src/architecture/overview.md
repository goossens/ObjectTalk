The ObjectTalk ecosystem is contained in a single executable file that embeds all subsystems and
their resources/assets together in one unit. The original design called for a simple executable
that would load shared modules and assets on demand. It was decided not to continue with this
approach as dealing with that DLL hell on Windows, keeping versions synchronized and the need
to put things all over the filesystem would lead to fragmentation and possibly missing/incompatible pieces.

Today, the ObjectTalk ecosystem compiles into a single file that contains all the libraries,
resources and assets. The file “ot”, which contains everything, can be moved around with easy and
will always be complete and in sync. During the compilation of ObjectTalk it is decided whether
the user interface subsystems are included. On headless Linux systems, they are omitted and
the “ot” executable can only run simple scripts or scripts that can be used to build a server.
When the GUI libraries are available on the host system, “ot” is compiled to offer all subsystems.
On MacOS and Windows this is the default but even here a stripped-down version can be compiled
by setting a flag during compilation.

Github is used as the [source code repository](https://github.com/goossens/ObjectTalk) and on
every hand-in, the latest versions of the “ot” executable are built for MacOS, Linux and Windows
and can be downloaded. Stable versions will also be made available from the same Github repository.
Documentation for ObjectTalk is also included in the repository and the
[documentation website](https://goossens.github.io/ObjectTalk) is automatically updated on every
hand-in (off course the developer has to ensure the documentation is inline with the software.
Documentation still doesn’t write itself).
