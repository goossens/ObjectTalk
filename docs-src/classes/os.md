The OS (Operating System) class encapsulates an interface to
the native operating system. Filesystem operations are not
included as they are covered by the [FS (File System)](#fs) class.
The [Global](#global) scope predefines an OS instance under the
name **os**.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| clock() | Return the number of seconds since the epoch as a floating point number. |
| cores() | Return an array of dictionaries giving details about the CPUs cores. |
| networks() | Return an array of dictionaries giving details about the network connections. |
| totalMemory() | Get the total available memory on this machine (in bytes). |
| freeMemory() | Get the amount of free memory available in the system (in bytes). |
| getDay() | Get the day in the month (1-31). |
| getDayOfWeek() | Get the day of the week (0-6). Sunday is day 0. |
| getDayOfYear() | Get day of the year (1-366). |
| getHours() | Get hours (0-23). |
| getMinutes() | Get minutes (0-59). |
| getMonth() | Get month (1-12). |
| getSeconds() | Get seconds (0.59). |
| getYear() | Get year. |
| isDST() | Is daylight savings time active. |
| getenv(name) | Get value of named environment variable. |
| hasenv(name) | See if named environment variable exists. |
| setenv(name, value) | Set named environment variable to provided value. |
| unsetenv(name) | Remove the named environment variable. |
| hostname() | Return the computer's hostname. |
| machine() | Return the hardware identification for the system. |
| sysname() | Return the Operating System name. |
| release() | Return the Operating System release. |
| version() | Return the Operating System version. |
| uuid() | Return a Universally Unique Identifier (UUID). |
| sleep() | Causes the calling thread to sleep for a specified number of milliseconds. |
| uptime() | Return the systems uptime in seconds. |
| runServer() | Run the event loop for a server app. |
| stopServer() | Stop the event loop for a server app. |
| runGUI() | Run the event loop for a GUI app. |
| stopGUI() | Stop the event loop for a GUI app. |
