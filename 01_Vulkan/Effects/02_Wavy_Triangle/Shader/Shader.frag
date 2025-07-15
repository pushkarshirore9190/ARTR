#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 out_Color;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform UBO {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    float time;
    vec3 padding;
} ubo;

void main(void)
{
    float r = abs(sin(ubo.time));
    float g = abs(sin(ubo.time + 2.0));
    float b = abs(sin(ubo.time + 4.0));

    FragColor = vec4(out_Color.rgb * vec3(r, g, b), 1.0);
}


