#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform MyuniformData
{
    mat4 mvpMatrix;
    vec4 No_Of_LineSegments;
    vec4 No_Of_LineStripes;
    vec4 Line_Color;
} myuniformData;

layout(location = 0) out vec4 FragColor;

void main(void)
{
	// code
	FragColor = myuniformData.Line_Color;
}






