# ObjectTalk Built-in Classes

Below is a list of classes included in the default ObjectClass runtime.
For each class, reference documentation is available. The classes marked
with an asterisk cannot be instantiated. These classes are created within
the ObjectTalk engine for internal use. They are documented here for
completeness and some of them might be exposed to the user.

Language Classes:
=================

* [Object](reference/Object.md)
	* [Primitive](reference/Primitive.md)
		* [Boolean](reference/Boolean.md)
		* [Integer](reference/Integer.md)
		* [Real](reference/Real.md)
		* [String](reference/String.md)
		* [Function](reference/Function.md)

	* [Collection](reference/Collection.md)
		* [Array](reference/Array.md)
		* [Dict](reference/Dict.md)
		* [Set](reference/Set.md)

	* [Internal](reference/Internal.md) *
		* [Class](reference/Class.md) *
		* [Global](reference/Global.md) *
		* [Module](reference/Module.md) *
		* [BoundFunction](reference/BoundFunction.md) *
		* [CodeFunction](reference/CodeFunction.md) *
		* [Throw](reference/Throw.md) *

		* [Reference](reference/Reference.md) *
			* [MemberReference](reference/MemberReference.md) *
			* [StringReference](reference/StringReference.md) *
			* [ArrayReference](reference/ArrayReference.md) *
			* [DictReference](reference/DictReference.md) *
			* [ObjectReference](reference/ObjectReference.md) *
			* [ClassReference](reference/ClassReference.md) *
			* [StackReference](reference/StackReference.md) *
			* [CaptureReference](reference/CaptureReference.md) *

		* [Iterator](reference/Iterator.md) *
			* [ArrayIterator](reference/ArrayIterator.md) *
			* [DictIterator](reference/DictIterator.md) *
			* [SetIterator](reference/SetIterator.md) *

Module Classes:
===============

System module:
--------------

* [Object](reference/Object.md)
	* [System](reference/System.md)
		* [Path](reference/Path.md)
		* [OS](reference/OS.md)
		* [FS](reference/FS.md)

HTTP module:
-----------

* [Object](reference/Object.md)
	* [Http](reference/Http.md)
		* [URI](reference/URI.md)
		* [HttpRouter](reference/HttpRouter.md)
			* [HttpServer](reference/HttpServer.md)
		* [HttpRequest](reference/HttpRequest.md)
		* [HttpResponse](reference/HttpResponse.md)

	* [Internal](reference/Internal.md) *
		* [HttpNext](reference/HttpNext.md) *
		* [HttpNotFound](reference/HttpNotFound.md) *
		* [HttpTimer](reference/HttpTimer.md) *
