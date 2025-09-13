#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec2 out_TexCoord;

layout(binding = 0) uniform UBO
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	float time;
	vec3 padding;
} ubo;

void main(void)
{
	// code
	vec4 jitteredPos = vPosition;

	// stronger wave effect
	float waveSpeed = 7.0;      // faster
	float waveAmplitudeY = 0.25; // vertical waving
	float waveAmplitudeX = 0.20; // horizontal sway

	jitteredPos.y += sin(ubo.time * waveSpeed + vPosition.x * 10.0) * waveAmplitudeY;
	jitteredPos.x += cos(ubo.time * waveSpeed + vPosition.y * 8.0) * waveAmplitudeX;

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * jitteredPos;
	out_TexCoord = vTexCoord;
}
