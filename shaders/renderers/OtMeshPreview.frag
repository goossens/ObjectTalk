//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;

layout(location=0) out vec4 fragColor;

layout(std140, set=3, binding=0) uniform UBO {
	vec3 viewPosition;
	vec3 lightPosition;
	vec3 objectColor;
	vec3 lightColor;
};

// main function
void main() {
	// ambient
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	float diffuseStrength = 0.8;
	vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(lightPosition - viewPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	// specular
	float specularStrength = 0.54;
	vec3 viewDir = normalize(viewPosition - vPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(result, 1.0);
}
