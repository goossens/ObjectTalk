//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

SAMPLER2D(s_skySphereTexture, 0);

void main() {
	gl_FragColor = texture2D(s_skySphereTexture, v_texcoord0);
}
