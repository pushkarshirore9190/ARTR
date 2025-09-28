#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec4 vPosition;
layout(location=1) in vec3 vNormal;

layout(binding=0) uniform UniformData
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

    vec4 lightAmbient1;
    vec4 lightDiffuse1;
    vec4 lightSpecular1;
    vec4 lightPosition1;

    vec4 lightAmbient2;
    vec4 lightDiffuse2;
    vec4 lightSpecular2;
    vec4 lightPosition2;

    vec4 lightAmbient3;
    vec4 lightDiffuse3;
    vec4 lightSpecular3;
    vec4 lightPosition3;

    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    float materialShininess;

    uint LKeyIsPressed;
} u_UniformData;

layout(location=0) out vec3 out_transformedNormal;
layout(location=1) out vec3 out_lightDirection1;
layout(location=2) out vec3 out_lightDirection2;
layout(location=3) out vec3 out_lightDirection3;
layout(location=4) out vec3 out_viwerVector;

void main(void)
{
    gl_Position = u_UniformData.projectionMatrix * u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;

    if(u_UniformData.LKeyIsPressed == 1)
    {
        vec4 eyeCoordinates = u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;
        mat3 normalMatrix = mat3(transpose(inverse(u_UniformData.viewMatrix * u_UniformData.modelMatrix)));
        out_transformedNormal = normalMatrix * vNormal;

        out_lightDirection1 = vec3(u_UniformData.lightPosition1) - eyeCoordinates.xyz;
        out_lightDirection2 = vec3(u_UniformData.lightPosition2) - eyeCoordinates.xyz;
        out_lightDirection3 = vec3(u_UniformData.lightPosition3) - eyeCoordinates.xyz;

        out_viwerVector = -eyeCoordinates.xyz;
    }
}


