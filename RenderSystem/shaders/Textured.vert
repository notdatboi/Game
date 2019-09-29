#version 460 core
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_tessellation_shader : enable

layout(location = 0) in vec4 posAndU;
layout(location = 1) in vec4 normalAndV;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec4 bitangent;

layout(set = 0, binding = 0) uniform VP
{
    mat4 view;
    mat4 proj;
} vp;

layout(set = 1, binding = 0) uniform M
{
    mat4 model;
} m;

layout(location = 0) out vec2 uv;

void main(void)
{
    uv = vec2(posAndU.w, normalAndV.w);
    uv.y = 1 - uv.y;
    gl_Position = vp.proj * vp.view * m.model * vec4(posAndU.xyz, 1);
    gl_Position.y = -gl_Position.y;
}