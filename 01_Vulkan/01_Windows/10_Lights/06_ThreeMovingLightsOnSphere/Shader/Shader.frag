#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 out_transformedNormal;
layout(location=1) in vec3 out_lightDirection1;
layout(location=2) in vec3 out_lightDirection2;
layout(location=3) in vec3 out_lightDirection3;
layout(location=4) in vec3 out_viwerVector;

layout(location = 0) out vec4 FragColor;

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

void main(void)
{
    vec3 phong_ads_light;

    if(u_UniformData.LKeyIsPressed == 1)
    {
        vec3 N = normalize(out_transformedNormal);
        vec3 V = normalize(out_viwerVector);

        // --- Light 1 ---
        vec3 L1 = normalize(out_lightDirection1);
        vec3 R1 = reflect(-L1, N);
        vec3 ambient1 = vec3(u_UniformData.lightAmbient1) * vec3(u_UniformData.materialAmbient);
        vec3 diffuse1 = vec3(u_UniformData.lightDiffuse1) * vec3(u_UniformData.materialDiffuse) * max(dot(L1, N), 0.0);
        vec3 specular1 = vec3(u_UniformData.lightSpecular1) * vec3(u_UniformData.materialSpecular) *
                         pow(max(dot(R1, V), 0.0), u_UniformData.materialShininess);

        // --- Light 2 ---
        vec3 L2 = normalize(out_lightDirection2);
        vec3 R2 = reflect(-L2, N);
        vec3 ambient2 = vec3(u_UniformData.lightAmbient2) * vec3(u_UniformData.materialAmbient);
        vec3 diffuse2 = vec3(u_UniformData.lightDiffuse2) * vec3(u_UniformData.materialDiffuse) * max(dot(L2, N), 0.0);
        vec3 specular2 = vec3(u_UniformData.lightSpecular2) * vec3(u_UniformData.materialSpecular) *
                         pow(max(dot(R2, V), 0.0), u_UniformData.materialShininess);

        // --- Light 3 ---
        vec3 L3 = normalize(out_lightDirection3);
        vec3 R3 = reflect(-L3, N);
        vec3 ambient3 = vec3(u_UniformData.lightAmbient3) * vec3(u_UniformData.materialAmbient);
        vec3 diffuse3 = vec3(u_UniformData.lightDiffuse3) * vec3(u_UniformData.materialDiffuse) * max(dot(L3, N), 0.0);
        vec3 specular3 = vec3(u_UniformData.lightSpecular3) * vec3(u_UniformData.materialSpecular) *
                         pow(max(dot(R3, V), 0.0), u_UniformData.materialShininess);

        // Sum all three lights
        phong_ads_light = ambient1 + diffuse1 + specular1 +
                          ambient2 + diffuse2 + specular2 +
                          ambient3 + diffuse3 + specular3;
    }
    else
    {
        phong_ads_light = vec3(1.0, 1.0, 1.0);
    }

    FragColor = vec4(phong_ads_light, 1.0);
}


