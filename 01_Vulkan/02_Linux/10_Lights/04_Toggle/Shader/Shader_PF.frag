#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 out_transformedNormal;
layout(location=1) in vec3 out_lightDirection;
layout(location=2) in vec3 out_viewerVector;

layout(location=0) out vec4 FragColor;

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

void main(void)
{
    vec3 phongLight = vec3(1.0, 1.0, 1.0);

    if(u_UniformData.LKeyIsPressed == 1)
    {
        vec3 N = normalize(out_transformedNormal);
        vec3 L = normalize(out_lightDirection);
        vec3 V = normalize(out_viewerVector);

        vec3 ambient = vec3(u_UniformData.lightAmbient) * vec3(u_UniformData.materialAmbient);
        vec3 diffuse = vec3(u_UniformData.lightDiffuse) * vec3(u_UniformData.materialDiffuse) * max(dot(L, N), 0.0);
        vec3 R = reflect(-L, N);
        vec3 specular = vec3(u_UniformData.lightSpecular) * vec3(u_UniformData.materialSpecular) *
                        pow(max(dot(R, V), 0.0), u_UniformData.materialShininess);

        phongLight = ambient + diffuse + specular;
    }

    FragColor = vec4(phongLight, 1.0);
}


