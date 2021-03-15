//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

Language classes:
=================

	Object
		Primitive
			Boolean
			Integer
			Real
			String
			Function

		Collection
			Array
			Dict

		Internal
			Context
				Module
				Class
				Global

			Reference
				ContextReference
				MemberReference
				StringReference
				ArrayReference
				DictReference

			Iterator
				StringIterator
				ArrayIterator

			BoundFunction
			CodeFunction
			Throw

Module classes:
===============

System module:
--------------

	Object
		System
			Path
			OS
			FS

Http module:
-----------

	Object
		Http
			URI
			HttpRouter
				HttpServer

		Internal
			HttpRequest
			HttpResponse
			HttpNext
			HttpNotFound
			HttpTimer
