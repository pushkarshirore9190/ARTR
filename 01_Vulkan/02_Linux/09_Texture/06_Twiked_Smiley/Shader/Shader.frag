#version 450 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 out_TexCoord;

layout(binding = 0) uniform MVPMatrix
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    int keyPressed;
} uMVP;

layout(binding = 1) uniform sampler2D uTextureSampler;

void main(void)
{
    if(uMVP.keyPressed == 0)
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // white
    }
    else
    {
        FragColor = texture(uTextureSampler, out_TexCoord);
    }
}


