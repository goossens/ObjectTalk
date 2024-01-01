//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting


#ifndef OT_TERRAIN_UNIFORMS_GLSL
#define OT_TERRAIN_UNIFORMS_GLSL


#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_terrain[9];

#define u_hScale u_terrain[0].x
#define u_vScale u_terrain[0].y
#define u_vOffset u_terrain[0].z
#define u_heightMapSize u_terrain[0].w

#define u_region1TextureSize u_terrain[1].x
#define u_region2TextureSize u_terrain[1].y
#define u_region3TextureSize u_terrain[1].z
#define u_region4TextureSize u_terrain[1].w

#define u_region1TextureScale u_terrain[2].x
#define u_region2TextureScale u_terrain[2].y
#define u_region3TextureScale u_terrain[2].z
#define u_region4TextureScale u_terrain[2].w

#define u_region1Transition u_terrain[3].x
#define u_region2Transition u_terrain[3].y
#define u_region3Transition u_terrain[3].z
#define u_textureNoise u_terrain[3].w

#define u_region1Overlap u_terrain[4].x
#define u_region2Overlap u_terrain[4].y
#define u_region3Overlap u_terrain[4].z

#define u_regionColor(i) u_terrain[i + 4].rgb
#define u_regionTextured(i) bool(u_terrain[i + 4].a)

// texture samplers
SAMPLER2D(s_normalmapSampler, 0);
SAMPLER2D(s_region1Sampler, 1);
SAMPLER2D(s_region2Sampler, 2);
SAMPLER2D(s_region3Sampler, 3);
SAMPLER2D(s_region4Sampler, 4);

#endif
