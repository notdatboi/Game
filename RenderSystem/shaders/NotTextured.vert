#version 460 core
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_tessellation_shader : enable

layout(location = 0) in vec4 posAndNormX;
layout(location = 1) in vec4 tanAndNormY;
layout(location = 2) in vec4 btanAndNormZ;

layout(set = 0, binding = 0) uniform VP
{
    mat4 view;
    mat4 proj;
} vp;

layout(set = 1, binding = 0) uniform M
{
    mat4 model;
} m;

void main(void)
{
    gl_Position = vp.proj * vp.view * m.model * vec4(posAndNormX.xyz, 0);
}