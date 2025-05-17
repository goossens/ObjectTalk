The **http.Router** class provides routing mechanisms to let a web server's
endpoints (URLs) respond to client requests. Routing is achieved by using
methods of the **http.Router** class. For instance, **http.Router.get** handles
an HTTP GET request and  **http.Router.post** handles an HTTP POST request.
You can find a list below of all the request types. You can also use
**http.Router.all** to handle all HTTP methods and use **http.Router.use**
to specify middleware as the callback function.

All routing methods specify a callback function (sometimes called “handler functions”)
called when the web server receives a request to the specified route (endpoint) and HTTP method.
In other words, the web server “listens” for requests that match the specified route(s) and method(s),
and when it detects a match, it calls the specified callback function.

The following code is an example of a very basic route:

	var http = import("http");
	var router = http.Router();

	// respond with "Hello World" when a GET request is made to the homepage
	router.get("/", function(req, res, next) {
		res.send("Hello World");
	});

**Route Methods**

A route method is derived from one of the HTTP methods, and is attached to an instance of the **http.Router** class.
The following code is an example of routes that are defined for the GET and the POST methods to the root of the web server.

	// GET method route
	router.get("/", function(req, res, next) {
		res.send("GET request to the homepage");
	});

	// POST method route
	router.post("/", function(req, res, next) {
		res.send("POST request to the homepage");
	});

The **http.Router** class supports methods that correspond to all HTTP request methods: get, post, and so on.
For a full list, see below. There is a special routing method, **http.Router.all**, used to load middleware
functions at a path for all HTTP request methods. For example, the following handler is executed for requests
to the route "/secret" whether using GET, POST, PUT, DELETE, or any other HTTP request method.

	router.all("/secret/*", function(req, res, next) {
		print("Accessing the secret section ...");
		next(); // pass control to the next handler
	});

**Route Paths**

Route paths, in combination with a request method, define the endpoints at which requests can be made.
Route paths can be strings, string patterns, or regular expressions. In case the pattern is a string,
the pattern will only match if the string and path are identical.

	// matches root route
	router.get("/", function(req, res, next) {
		res.send("root");
	});

	// matches /about route
	router.get("/about", function(req, res, next) {
		res.send("about");
	});

	// matches /random.text route
	router.get("/random.text", function(req, res, next) {
		res.send("random.text");
	});

A string pattern ends with a \* character (the wild card) and all paths that start
with the same character sequence

	// matches all paths that start with "/secret/"
	router.all("/secret/*", function(req, res, next) {
		res.send("in secret area");
	});

Path matching based on regular express is still on the TODO list.

**Route Parameters**

Route parameters are named URL segments that are used to capture the values specified
at their position in the URL. The captured values are populated in the req object,
with the name of the route parameter specified in the path as their respective keys.

	Route path: /users/:userId/books/:bookId
	Request URL: http://localhost:3000/users/34/books/8989
	req.getParams(): { "userId": "34", "bookId": "8989" }

To define routes with route parameters, simply specify the route parameters in the path of the
route as shown below.

	router.all("/users/:userId/books/:bookId", function(req, res, next) {
		res.send(req.params);
	});

**Route Handlers**

A route handler or callback is a function that must accept three parameters:
req(uest), res(pond) and next. The first is the request object (which is an
instance of **http.Request**) containing details about the request. The second
is the respond object (which is an instance of **http.Respond**) which is used
to tell the web server what to send back. The third is the next object which
must be executed to pass the request to the next route handler.

**Static Handler**

To serve static content, a static handler can be attached to a route. This handler
matches the start of a URL path to a filesystem path. This makes it quick
to serve up hierarchies of files using a UNIX mount style approach.

	// serve up the contents of the local 'static' directory under the /static HTTP path
	router.static("/static", __DIR__.join("static"));

The static handler protects against con-artist that try to escape from
the specified directory by using ".." path segments.

**Timer Handler**

The **http.Router** class also provides a timer handler which is called
at a specified interval. This can be useful for dynamic web servers that
have to update state after certain time intervals.

	// run function every 10 seconds
	router.timer(10000, function() {
		// update state
	});

**Member Functions**

| Function | Description |
| ------ | ----------- |
| use(handler) | Respond to all HTTP request. |
| all(path, handler) | Respond to all HTTP methods for specified path. |
| get(path, handler) | Respond to HTTP GET methods for specified path. |
| put(path, handler) | Respond to HTTP PUT methods for specified path. |
| post(path, handler) | Respond to HTTP POST methods for specified path. |
| delete(path, handler) | Respond to HTTP DELETE methods for specified path. |
| static(path, ospath) | Mount specified OS path at specified URL path. |
| timer(interval, callback) | Call specified callback after specified interval (in milliseconds). |
