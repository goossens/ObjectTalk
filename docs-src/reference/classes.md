Below is a list of classes included in the default ObjectClass runtime.
For each class, reference documentation is available. The classes marked
with an asterisk cannot be instantiated. These classes are created within
the ObjectTalk engine for internal use. They are documented here for
completeness and some of them might be exposed to the user.

## Core Classes:

* [Object](#object)
	* [Primitive](#primitive)
		* [Boolean](#boolean)
		* [Integer](#integer)
		* [Real](#real)
		* [String](#string)
		* [Function](#function)
	* [Collection](#collection)
		* [Array](#array)
		* [Dict](#dict)
		* [Set](#set)
	* [Internal](#internal) *
		* [Class](#class) *
		* [Global](#global) *
		* [Module](#module) *
		* [BoundFunction](#boundfunction) *
		* [CodeFunction](#vodefunction) *
		* [Throw](#Throw) *
		* [Reference](#reference) *
			* [MemberReference](#memberreference) *
			* [StringReference](#stringreference) *
			* [ArrayReference](#arrayreference) *
			* [DictReference](#dictreference) *
			* [ObjectReference](#objectreference) *
			* [ClassReference](#classreference) *
			* [StackReference](#stackreference) *
			* [CaptureReference](#capturereference) *
		* [Iterator](#iterator) *
			* [ArrayIterator](#arrayiterator) *
			* [DictIterator](#dictiterator) *
			* [SetIterator](#setiterator) *

## System Classes


* [Object](#object)
	* [System](#system)
		* [Path](#path)
		* [OS](#os)
		* [FS](#fs)

## HTTP Classes

* [Object](#object)
	* [Http](#http)
		* [URI](#uri)
		* [HttpRouter](#httprouter)
			* [HttpServer](#httpserver)
		* [HttpRequest](#httprequest)
		* [HttpResponse](#httpresponse)
	* [Internal](#internal) *
		* [HttpNext](#httpnext) *
		* [HttpNotFound](#httpnotfound) *
		* [HttpTimer](#httptimer) *
