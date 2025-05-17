The **http.Url** class provides support for URL parsing and resource
downloading. The parser is compliant with RFC 1738 and understands the following syntax:

**URL:**

![URL](img/url.png)

	URL ::= scheme ":" authority? path ("?" query)? ("#" fragment)?

**Authority:**

![Authority](img/authority.png)

	authority ::= "//" (username (":" password)? "@")? host (":" port)?

To use the **http.Url** class to download resources, it is as simple as:

	var http = import("http");
	var url = http.Url("http://host.name/resource-path");
	var content = url.download();

	// or as a one-liner
	var content = import("http").Url("http://host.name/resource-path").download();

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init\_\_(url) | Create a URL object form the provided string. |
| url() | Return the full URL as a string. Returns "" if not specified. |
| scheme() | Return the scheme part as a string. Returns "" if not specified. |
| authority() | Return the authority part as a string. Returns "" if not specified. |
| user() | Return the user part as a string. Returns "" if not specified. |
| password() | Return the password part as a string. Returns "" if not specified. |
| host() | Return the host part as a string. Returns "" if not specified. |
| port() | Return the port part as an integer. Returns 80 if not specified. |
| path() | Return the path part as a string. Returns "" if not specified. |
| directory() | Return the directory part of the path as a string. Returns "" if not specified. |
| filename() | Return the filename part of the path as a string. Returns "" if not specified. |
| stem() | Return the stem part of the filename as a string. Returns "" if not specified. |
| extension() | Return the extension part of the filename as a string. Returns "" if not specified. |
| query() | Return the query part as a string. Returns "" if not specified. |
| fragment() | Return the fragment part as a string. Returns "" if not specified. |
| hasParam(parameter) | Determines if the query part contains the specified parameter. |
| getParam(parameter) | Return the specified parameter as a string. Returns "" if not specified. |
| getParamWithDefault(parameter, default) | Return the specified parameter as a string. Returns provided default if not specified. |
| download() | Download the resource addressed by the URL and return it as a string. If something goes wrong, an exception is raised. |
| getStatus() | Return the HTTP status code from the last download request. |
