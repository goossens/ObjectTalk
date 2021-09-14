//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

SAMPLERCUBE(s_cubemap, 0);

void main() {
	gl_FragColor = textureCube(s_cubemap, v_texcoord0);
}
