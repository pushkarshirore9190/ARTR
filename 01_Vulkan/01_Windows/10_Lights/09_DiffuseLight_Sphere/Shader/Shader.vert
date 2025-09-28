#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec4 vPosition;
layout(location=1) in vec3 vNormal;

layout(binding=0) uniform UniformData
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

    vec4 lightDiffused;
    vec4 lightPosition;
    vec4 materialDiffused;

    uint LKeyIsPressed;

} u_UniformData;

layout(location=0) out vec4 out_diffuse_light;

void main(void)
{
    gl_Position = u_UniformData.projectionMatrix * u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;

    if(u_UniformData.LKeyIsPressed == 1)
    {
        vec4 eyeCoordinates = u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;
        mat3 normalMatrix = mat3(transpose(inverse(u_UniformData.viewMatrix * u_UniformData.modelMatrix)));
        vec3 transformedNormals = normalize(normalMatrix * vNormal);
        vec3 lightSource = normalize(vec3(u_UniformData.lightPosition - eyeCoordinates));

        float NdotL = max(dot(transformedNormals, lightSource), 0.0);
        vec3 diffuse = vec3(u_UniformData.lightDiffused) * vec3(u_UniformData.materialDiffused) * NdotL;

        out_diffuse_light = vec4(diffuse, 1.0);
    }
    else
    {
        out_diffuse_light = vec4(1.0, 1.0, 1.0, 1.0);
    }
}


