#version 460 core
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_tessellation_shader : enable

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 tanAndU;
layout(location = 2) in vec4 btanAndV;

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
    uv = vec2(tanAndU.w, btanAndV.w);
    uv = vec2(uv.x, 1 - uv.y);
    gl_Position = vp.proj * vp.view * m.model * vec4(pos.xyz, 0);
}