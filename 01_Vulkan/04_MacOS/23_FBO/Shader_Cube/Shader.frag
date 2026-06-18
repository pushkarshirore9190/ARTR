#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 out_TexCoord;

layout(binding = 1) uniform sampler2D uTextureSampler;

void main(void)
{
	// code
	FragColor = texture(uTextureSampler,out_TexCoord);
}






