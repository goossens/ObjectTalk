//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting


#ifndef OT_TERRAIN_UNIFORMS_GLSL
#define OT_TERRAIN_UNIFORMS_GLSL


#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_terrain[7];

#define u_size u_terrain[0].x
#define u_step u_terrain[0].y

#define u_regionTransition1 u_terrain[1].x
#define u_regionTransition2 u_terrain[1].y
#define u_regionTransition3 u_terrain[1].z
#define u_scale u_terrain[1].w

#define u_regionOverlap1 u_terrain[2].x
#define u_regionOverlap2 u_terrain[2].y
#define u_regionOverlap3 u_terrain[2].z

#define u_regionColor(i) u_terrain[i + 2].rgb
#define u_regionTextured(i) bool(u_terrain[i + 2].a)

// texture samplers
SAMPLER2D(s_region1Sampler, 0);
SAMPLER2D(s_region2Sampler, 1);
SAMPLER2D(s_region3Sampler, 2);
SAMPLER2D(s_region4Sampler, 3);
SAMPLER2D(s_normalmapSampler, 4);

#endif
