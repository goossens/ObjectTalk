The Worker class provides a mechanism to offload tasks to another
thread. When creating a worker, a function is provided that performs
the work. This function must be able to receive an object (the task)
and return the result.

To run a job a task is passed with a callback function. This callback
is called with the request (the task) and the response (the result)
when the worker is done.

The main thread must call the update function until the worker
reports all the work done.

Here is a trivial example:

	var worker = Worker(function(input) {
		return input.sqrt() * input.cos().abs();
	});

	var total = 0.0;

	for i in range(100) {
		worker.run(1000.0.random(), function(request, response) {
			total += response;
		});
	}

	while (!worker.done()) {
		worker.update();
	}

	print(total);

Obviously, workers are more valuable when run in parallel with other
workers or with intensive operations like 3D apps with high frame
rates.

**Member Functions**

| Function | Description |
| ------ | ----------- |
| \_\_init__(function) | Initialize a Worker object by providing the function that will do the heavy lifting. The function must take 1 parameter (the request) and return the result. |
| run(request, callback) | Add a new request to the worker's queue. Once the worker is done, the callback will be called with 2 parameters (the request and the response). |
| requests() | Return the number of open requests. |
| update() | This function must be called until all worker are done. |
| done() | Returns true when all work is done and processed. |
