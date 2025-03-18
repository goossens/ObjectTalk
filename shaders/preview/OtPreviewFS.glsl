//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal

#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_preview[4];
#define u_viewPos u_preview[0].xyz
#define u_lightPos u_preview[1].xyz
#define u_ObjectColor u_preview[2].rgb
#define u_lightColor u_preview[3].rgb

// main function
void main() {
	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * u_lightColor;

	// diffuse
	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(u_lightPos - v_position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(u_viewPos - v_position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * u_lightColor;

	vec3 result = (ambient + diffuse + specular) * u_ObjectColor;
    gl_FragColor = vec4(result, 1.0);
}
