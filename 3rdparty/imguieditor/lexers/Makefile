#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

%.h : %.re
	re2c -i -o $@ $<

lexers: c.h cs.h json.h lua.h markdown.h python.h
