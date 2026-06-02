#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform MyuniformData
{
    mat4 mvpMatrix;
    vec4 No_Of_LineSegments;
    vec4 No_Of_LineStripes;
    vec4 Line_Color;
} myuniformData;


layout (isolines) in;

void main(void)
{
	vec3 P0 = gl_in[0].gl_Position.xyz;
	vec3 P1 = gl_in[1].gl_Position.xyz;
	vec3 P2 = gl_in[2].gl_Position.xyz;
	vec3 P3 = gl_in[3].gl_Position.xyz;

	vec3 P = P0 * (1 - gl_TessCoord.x) * (1 - gl_TessCoord.x) * (1 - gl_TessCoord.x) + 
			 P1 * 3 * gl_TessCoord.x * (1 - gl_TessCoord.x) * (1 - gl_TessCoord.x) +
			 P2 * 3 * gl_TessCoord.x * gl_TessCoord.x * (1 - gl_TessCoord.x) +
			 P3 * gl_TessCoord.x * gl_TessCoord.x * gl_TessCoord.x;

	gl_Position = myuniformData.mvpMatrix * vec4(P,1.0f);
   
}





