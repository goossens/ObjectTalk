A module is a single unit of code that’s built and shipped as a single unit
and that can be imported by another module with ObjectTalk's import keyword.

A source file is a single ObjectTalk module (in effect, a single file within
an app). Although it’s common to define individual types in separate source
files, a single source file can contain definitions for multiple classes,
functions, and so on.

When the ObjectTalk interpreter is started, it will load the specified
script (source code) as the first module. Additional modules can be
loaded with the import script. Here is an example based on two files; one
containing a class definition and the other containing the main program.

	// Module "counter.ot"

	class Counter : Object {
		function__init__(this) {
			this.count = 0;
		}

		function increment(this) {
			this.count += 1;
		}
		function increment(this, amount) {
			this.count += amount;
		}
		function reset(this) {
			this.count = 0;
		}
	}

	// Module "app.ot"

	var counter = import("counter");

	var tracker = counter.Counter(); // the initial counter value is 0
	assert(tracker.count == 0);

	tracker.increment();             // the counter's value is now 1
	assert(tracker.count == 1);

	tracker.increment(5);            // the counter's value is now 6
	assert(tracker.count == 6);

	tracker.reset();                 // the counter's value is now 0
	assert(tracker.count == 0);
