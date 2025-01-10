//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_color0, v_texcoord0

#include <bgfx_shader.glsl>

SAMPLER2D(s_imguiFontAtlas, 0);

void main() {
	vec4 texel = texture2D(s_imguiFontAtlas, v_texcoord0);
	gl_FragColor = texel * v_color0;
}
