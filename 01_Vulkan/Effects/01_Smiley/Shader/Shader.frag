#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 out_TexCoord;

layout(binding = 1) uniform sampler2D uTextureSampler;

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
	vec4 texColor = texture(uTextureSampler, out_TexCoord);

	// animated RGB modulation
	float r = abs(sin(ubo.time * 2.0));
	float g = abs(sin(ubo.time * 2.0 + 2.0));
	float b = abs(sin(ubo.time * 2.0 + 4.0));

	FragColor = vec4(texColor.rgb * vec3(r, g, b), texColor.a);
}
