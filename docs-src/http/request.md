The **http.Request** class contains information on the current HTTP request
and it's passed to a route handler by the **http.Router**.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| getMethod() | Return the request's HTTP method. |
| getURL() | Return the request's URL. |
| getPath() | Return the request's path. |
| getVersion() | Return the HTTP protocol version. |
| hasHeader(name) | Determine if specified header was provided by client. |
| headerIs(name, value) | Determine if specified header has specified value. |
| getHeader(name) | Return specified header as a string. Returns "" if not found. |
| getHeaders(name) | Return all headers as a dictionary. |
| hasParam(name) | Determine if specified parameter was provided by client. |
| getParam(name) | Return specified parameter as a string. Returns "" if not found. |
| getParams(name) | Return all parameters as a dictionary. |
| hasCookie(name) | Determine if specified cookie was provided by client. |
| getCookie(name) | Return specified cookie as a string. Returns "" if not found. |
| getBody() | Return body of request as string. |
