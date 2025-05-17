The **http.Server** class instantiates a web server on a specified port using a
specified request router. Those of you familiar with NodeJS or Python's
Twisted Framework will probably recognize the inspiration for these
ObjectTalk classes. In general, the ObjectTalk web server is a lot simpler
as the aforementioned packages but the purpose of the ObjectTalk classes
is not to compete with those but rather to educate and provide simple solution
to non-complex problems.

As an example, here is a simple "Hello World" web server in ObjectTalk.
It runs on port 80 and responds with "Hello World" to every HTTP GET request
on all URLs.

	var http = import("http");

	var router = http.Router()
		.get("*", function(req, res, next) {
			res.send("Hello World");
		});

	var server = http.Server(router).listen("0.0.0.0", 80);
	os.runServer();

The **http.Server** server uses a request router to process all incoming requests.
The **listen** member function starts the server on the speficied port
using the specified host. In the example above, requests can come from any host.

The **http.Server** class does not have its own event loop. If a server is running from the
command line, an event loop must be created using the **os.runServer();** command
as in the example above. If the server in running in a GUI application (yes, that's possible),
you don't have to do anything as every GUI appication already has an event loop.
By not including an event loop in the  **http.Server** class, the possibility exists to
run multiple network servers in a single ObjectTalk instance.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init\_\_(router) | Create an HTTP server using the specified HTTP **Router** for processing requests. |
| listen(host, port) | Start the HTTP server using the specified host and port specification. |
