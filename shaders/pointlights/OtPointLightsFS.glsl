//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0, v_color0

#include <bgfx_shader.glsl>
#include <lighting.glsl>
#include <pbr.glsl>
#include <utilities.glsl>

// texture samplers
SAMPLER2D(s_lightingAlbedoTexture, 0);
SAMPLER2D(s_lightingNormalTexture, 1);
SAMPLER2D(s_lightingPbrTexture, 2);
SAMPLER2D(s_lightingDepthTexture, 3);

// main function
void main() {
	// determine light and vertex position (in world space)
	vec3 lightPosition = v_texcoord0;

	vec4 screen = gl_FragCoord;
	screen.z = texture2D(s_lightingDepthTexture, gl_FragCoord.xy / u_viewRect.zw).x;
	vec3 vertexPosition = screenToWorldSpace(screen);

	// determine light data
	PointLight light;
	light.radius = v_color0.a;
	light.color = v_color0.rgb;
	light.L = normalize(lightPosition - vertexPosition);
	light.attenuation = smoothAttenuation(distance(lightPosition, vertexPosition), light.radius);
	vec3 color = vec3_splat(0.0);

	if (light.attenuation > 0.0) {
		// determine material data
		Material material;
		vec2 texcoord = gl_FragCoord.xy / u_viewRect.zw;
		material.albedo = toLinear(texture2D(s_lightingAlbedoTexture, texcoord));
		material.normal = texture2D(s_lightingNormalTexture, texcoord).xyz * 2.0 - 1.0;
		vec4 data = texture2D(s_lightingPbrTexture, texcoord);
		material.metallic = data.r;
		material.roughness = data.g;
		material.ao = data.b;

		// determine view direction
		vec3 V = normalize(u_cameraPosition - vertexPosition);
		color = pointLightPBR(material, light, V);
	}

	gl_FragColor = vec4(color, 1.0);
}
