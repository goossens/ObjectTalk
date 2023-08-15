/	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_SHADERTOY_GLSL
#define OT_SHADERTOY_GLSL

uniform vec4 u_ShaderToy[6];

#define iResolution u_ShaderToy[0].rgb			// viewport resolution (in pixels)
#define iTime u_ShaderToy[0].a					// shader playback time (in seconds)
#define iTimeDelta u_ShaderToy[1].r				// render time (in seconds)
#define iFrame u_ShaderToy[1].g					// shader playback frame
#define iSampleRate u_ShaderToy[1].b			// sound sample rate (i.e., 44100)
#define iChannelTime u_ShaderToy[2]				// channel playback time (in sec)
#define iMouse u_ShaderToy[3]					// mouse pixel coords
#define iDate u_ShaderToy[4]					// (year, month, day, time in seconds)
#define iChannelResolution u_ShaderToy[5]		// channel resolution (in pixels)

SAMPLER2D(iChannel0, 0);
SAMPLER2D(iChannel1, 1);
SAMPLER2D(iChannel2, 2);
SAMPLER2D(iChannel3, 3);

#endif
