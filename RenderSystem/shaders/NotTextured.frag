#version 460 core
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_tessellation_shader : enable

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform Colors
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
} colors;

void main()
{
    outColor = colors.ambient;
    //outColor = vec4(1, 1, 0, 1);
}