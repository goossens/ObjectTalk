A closure is a function that captures and retains its surrounding environment's state,
including variables. This captured environment allows the closure to access and
modify those values even after the enclosing function has completed execution.
Capture is the mechanism by which closures "remember" and access their surrounding context.

When a closure is created, it captures any variables or constants defined in the
surrounding scope that it uses within its own body. These captured variables are
effectively "remembered" by the closure. Even after the enclosing function has
finished running, the closure's captured variables remain available to it.

In a simple case, a closure might capture a variable from its parent function
and increment it each time it is called, maintaining a state across multiple calls.
In another case, a closure can also capture objects or functions from its surrounding
context and use them later, even after the context has been deallocated.

	function f(x) {
		return function(y) {
			x + y;
		}
	}

	var g = f(1);
	assert(g(5) == 6);

	var h = f(10);
	assert(h(5) == 15);

In the example above, f returns a function that captures the value for x
which is then used later when the function is executed.
