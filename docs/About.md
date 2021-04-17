# About ObjectTalk

![Logo](../logo/logo-small.png)

ObjectTalk is a simple object-oriented scripting language that has
evolved over decades and was heavily influenced by other languages.
As a long time IT professional, I've seen a lot of software development
with all its greatness and flaws. ObjectTalk represents what I think
is a well-balanced scripting language that is easy to learn and can
make even a novice look good quickly. If you are interested in how I got
to where we are today, please see the [history](History.md) page
for the story behind ObjectTalk.

So before continuing, let me address the elephant in the room. Why the
heck would the world need another scripting language? Well, the simple
answer is: it doesn't as we already have more than enough of them! I
wouldn't even suggest ObjectTalk is a language for serious software
development. If you need to deploy code, look elsewhere. ObjectTalk is
however is a good starting point to learn programming and it's also good
if you want to learn language development. Those of you who have ever
looked at the code for compilers and interpreters of populars
languages will probably know what I mean.

The objectives for the development of ObjectTalk where:

* Learn computer language development.
* Learn how to write a lexical scanner and language compiler.
* Learn how to implement a simple stack-based virtual machine.
* Learn how the develop a language that is:
	* Fully object-oriented.
	* Dynamically typed.
	* Lexically scoped.
	* Closure/capture aware.
* Learn C++17 as that is the language the ObjectTalk engine is written in.
* Keep the code multi-platform to support reuse and maximum outreach.
* Don't over-optimize to keep the code readable (so others can learn).
* End up with something that can be used for simple task.

So I believe (and yes, I might be a little biased), that ObjectTalk is
meeting the objectives above and will provide others the ability to
learn from this as well. If you agree with me, feel free to reuse and
contribute. Collective learning is a great thing.

So what's good about ObjectTalk? Well, typically it is a very personal
as to what is good or bad about a computer language. Many religious
arguments have been used over the years when it comes to good and
bad in Computerland. Those that have strong opinions about computer
languages will have the same views about ObjectTalk. I'm wide open
to ObjectTalk improvements but they'll have to be within the context
of the objectives above.

So what makes ObjectTalk great?

* It is truly object-oriented as everything is an object (even numbers,
strings and functions) with members and member functions.
* The language will feel comfortable to everyone who has worked on
C-derived languages.
* Variables are dynamically typed and primitive objects like Boolean,
Integer, Real and String are automatically converted if required.
* Functions are first class objects and can be passed as arguments or
bound to an object as a method.
* Variables are lexically scoped and must be declared before use.
* Variables are automatically captured in closures if required.
* Variables are always initialized before use.
* Memory is managed automatically.
