#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE OT_DOCS "*.md" "*/*.md")
file(GLOB_RECURSE OT_DOCS_TMPL "*.pug")
file(GLOB_RECURSE OT_DOCS_IMG "*.png")
file(GLOB_RECURSE OT_DOCS_EBNF "*.ebnf")

add_custom_target(docs SOURCES ${OT_DOCS} ${OT_DOCS_TMPL} ${OT_DOCS_IMG} ${OT_DOCS_EBNF})
