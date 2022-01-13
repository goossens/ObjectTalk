The FS (File System) class encapsulates an interface to
the native file system.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| capacity() | Return the total amount of space on the current file system in bytes. |
| available() | Return the amount of available space on the current file system in bytes. |
| free() | Return the amount of free space on the current file system in bytes. |
| gethome() | Return the current user's home directory. |
| getcwd() | Return the current working directory. |
| chdir(path) | Set the current working directory to specified path. |
| gettmp() | Return the path the the system's temporary directory. |
| tmpnam() | Return path for a temporary file. |
| mktmpdir() | Create a temporary directory and return path. |
| filesize(file) | Return the size of the specified file in bytes. |
| cp(from, to) | Copy filesystem object represented by a path (file or directory) to a new location. If object is a directory, the copy will be recursive. |
| mv(from, to) | Move filesystem object represented by a path (file or directory) to a new location. |
| mkdir(path) | Create a new directory. Parent path must exist. |
| mkdirs(path) | Create a new directory and all the intermediate directories if required. |
| ln(from, to) | Link a file system object to a new path. |
| lns(from, to) | symbolically link a file system object to a new path. |
| ls(directory) | Return an array of file system object names contains in specified directory. |
| resize(file, newSize) | Change the size of the file: if the file size was previously larger than newSize, the remainder of the file is discarded. If the file was previously smaller than newSize, the file size is increased and the new area appears as if zero-filled. |
| rm(file) | Delete the specified file. |
| rmdir(directory) | Delete the specified directory. If the directory is not empty, this operation will fail. |
| rmdirs(directory) | Delete the specified directory and its content recursively. |
| touch(file) | Touch a file by updating its last access time without changing the contents. |
