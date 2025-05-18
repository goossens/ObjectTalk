Objects of type path represent paths on a filesystem. A path name
has the following syntax:

* **root-name** (optional): identifies the root on a filesystem
with multiple roots (such as "C:" or "//myserver"). In case of
ambiguity, the longest sequence of characters that forms a valid
root-name is treated as the root-name.
* **root-directory** (optional): a directory separator that, if
present, marks this path as absolute. If it is missing (and the
first element other than the root name is a file name), then the
path is relative and requires another path as the starting location
to resolve to a file name.
* Zero or more of the following:
	* **file-name**: sequence of characters that aren't directory
	separators or preferred directory separators (additional
	limitations may be imposed by the OS or file system). This
	name may identify a file, a hard link, a symbolic link, or a
	directory. Two special file-names are recognized:
		* **dot**: the file name consisting of a single dot
		character . is a directory name that refers to the current
		directory
		* **dot-dot**: the file name consisting of two dot
		characters .. is a directory name that refers to the
		parent directory.
	* **directory-separators**: the forward slash character / or
	the alternative character provided by the operating system.
	If this character is repeated, it is treated as a single
	directory separator: /usr///////lib is the same as /usr/lib.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init__(...) | Construct a filesystem Path by combining all provided directories. An operating system directory separator is automatically inserted. |
| \_\_add__(operand) | Concatenate two paths without introducing a directory separator. |
| \_\_div__(operand) | Append elements to the path with a directory separator. |
| \_\_iter__() | Return a [PathIterator](#pathiterator) object to iterate through the directories in a path. This member function is called when you use the **for in** statement. You typically don't call this member function directly. |
| join() | Append elements to the path with a directory separator. |
| clear() | Erase the contents of the path. |
| exists() | See if the path exists on this system. |
| isAbsolute() | Check whether the path is absolute. |
| isRelative() | Check whether the path is relative. |
| isEmpty() | Check if the path is empty. |
| relative(operand) | Return relative path from operand. |
| directory() | Return the parent path. |
| filename() | Return the filename. |
| extension() | Return the file extension. |
| rootDirectory() | Return the root directory. |
| rootName() | Return the root name. |
| rootPath() | Return the root path. |
| stem() | Return the stem (filename without the final extension). |
| removeFilename() | Remove the filename component. |
| replaceExtension() | Replace the file extension. |
| replaceFilename() | Replace the filename. |
| hasExtension() | Check if the extension element of the path is not empty. |
| hasFilename() | Check if the filename element of the path is not empty. |
| hasParentPath() | Check if the parent element of the path is not empty. |
| hasRelativePath() | Check if the relative path element of the path is not empty. |
| hasRootDirectory() | Check if the root directory element of the path is not empty. |
| hasRootName() | Check if the root name element of the path is not empty. |
| hasRootPath() | Check if the root path element of the path is not empty. |
| hasStem() | Check if the stem element of the path is not empty. |
| relative() | Return path relative to the specified base. |
| canonical() | Return the canonical absolute path. |

**Overrides**

The String class overrides the following member functions from the Object class:

* string
* \_\_eq__
* \_\_ne__
