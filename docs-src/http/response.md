The **http.Response** class provides an interface to respond to HTTP requests
and it's passed to a route handler by the **http.Router**.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| setStatus(status) | Set the HTTP respose status. |
| setHeader(name, value) | Set the value of a specified HTTP respose header. |
| hasHeader(name) | Determine if a specified HTTP respose header is already set. |
| end() | End the response. Useful for empty responses. |
| send(text) | Send the specified response to the client. |
| sendJson(json) | Send the specified JSON text to the client. |
| sendFile(filepath) | Send the specified file to the client. |
| sendFileToDownload(filepath) | Send the specified file to the client's download folder. |
