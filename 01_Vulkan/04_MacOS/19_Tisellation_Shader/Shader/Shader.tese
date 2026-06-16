#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform MyuniformData
{
    mat4 mvpMatrix;
    vec4 No_Of_LineSegments;
    vec4 No_Of_LineStripes;
    vec4 Line_Color;
} myuniformData;

layout (quads, equal_spacing, ccw) in;

void main(void)
{
    float t = gl_TessCoord.x;
    float side = gl_TessCoord.y * 2.0 - 1.0;

    vec3 P0 = gl_in[0].gl_Position.xyz;
    vec3 P1 = gl_in[1].gl_Position.xyz;
    vec3 P2 = gl_in[2].gl_Position.xyz;
    vec3 P3 = gl_in[3].gl_Position.xyz;

    float u = 1.0 - t;

    vec3 P =
        P0 * u * u * u +
        P1 * 3.0 * t * u * u +
        P2 * 3.0 * t * t * u +
        P3 * t * t * t;

    vec3 tangent =
        3.0 * (P1 - P0) * u * u +
        6.0 * (P2 - P1) * u * t +
        3.0 * (P3 - P2) * t * t;

    vec2 n = normalize(vec2(-tangent.y, tangent.x));

    float lineWidth = 0.01;
    P.xy += n * side * lineWidth;

    gl_Position = myuniformData.mvpMatrix * vec4(P, 1.0);
}




