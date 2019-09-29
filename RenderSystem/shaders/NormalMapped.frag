#version 460 core
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_tessellation_shader : enable

layout(location = 0) in vec2 uv;

layout(set = 2, binding = 0) uniform Colors
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
} colors;

layout(set = 3, binding = 0) uniform sampler2D txt;

layout(set = 4, binding = 0) uniform sampler2D nMap;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(txt, uv) * colors.ambient;
    //outColor = vec4(1, 1, 1, 1);
}