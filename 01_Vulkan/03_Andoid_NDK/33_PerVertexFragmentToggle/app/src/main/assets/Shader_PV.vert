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

layout(location=0) out vec4 out_vertexColor;

void main(void)
{
    gl_Position = u_UniformData.projectionMatrix * u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;

    if(u_UniformData.LKeyIsPressed == 1)
    {
        vec4 eyeCoordinates = u_UniformData.viewMatrix * u_UniformData.modelMatrix * vPosition;
        mat3 normalMatrix = mat3(transpose(inverse(u_UniformData.viewMatrix * u_UniformData.modelMatrix)));
        vec3 transformedNormal = normalize(normalMatrix * vNormal);
        vec3 lightDirection = normalize(vec3(u_UniformData.lightPosition) - eyeCoordinates.xyz);
        vec3 viewerVector = normalize(-eyeCoordinates.xyz);

        vec3 ambient = vec3(u_UniformData.lightAmbient) * vec3(u_UniformData.materialAmbient);
        vec3 diffuse = vec3(u_UniformData.lightDiffuse) * vec3(u_UniformData.materialDiffuse) * max(dot(lightDirection, transformedNormal), 0.0);
        vec3 reflectionVector = reflect(-lightDirection, transformedNormal);
        vec3 specular = vec3(u_UniformData.lightSpecular) * vec3(u_UniformData.materialSpecular) *
                        pow(max(dot(reflectionVector, viewerVector), 0.0), u_UniformData.materialShininess);

        out_vertexColor = vec4(ambient + diffuse + specular, 1.0);
    }
    else
    {
        out_vertexColor = vec4(1.0,1.0,1.0,1.0);
    }
}


