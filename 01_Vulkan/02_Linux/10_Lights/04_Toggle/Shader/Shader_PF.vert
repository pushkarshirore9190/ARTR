#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec4 vPosition;
layout(location=1) in vec3 vNormal;

layout(binding=0) uniform UniformData
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

    vec4 lightAmbient;
    vec4 lightDiffuse;
    vec4 lightSpecular;
    vec4 lightPosition;

    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    float materialShininess;

    uint LKeyIsPressed;

} u_UniformData;

layout(location=0) out vec3 out_transformedNormal;
layout(location=1) out vec3 out_lightDirection;
layout(location=2) out vec3 out_viewerVector;

void main(void)
{
    gl_Position = u_UniformData.projectionMatrix * u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;

    if(u_UniformData.LKeyIsPressed == 1)
    {
        vec4 eyeCoordinates = u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;
        mat3 normalMatrix = mat3(transpose(inverse(u_UniformData.viewMatrix * u_UniformData.modelMatrix)));
        out_transformedNormal = normalMatrix * vNormal;
        out_lightDirection = vec3(u_UniformData.lightPosition) - eyeCoordinates.xyz;
        out_viewerVector = -eyeCoordinates.xyz;
    }
}


