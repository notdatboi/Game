#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define MAX_VERTEX_COUNT 100000
#define MAX_VERTEX_SIZE 64
#define MAX_IMAGE_DIMENSION 2048
#define MAX_TEXTURE_COUNT 10
#define MAX_UNIFORM_COUNT 15

enum DrawableType
{
    DTNotTextured,
    DTTextured,
    DTTexturedWithNormalMap,
    DTCount
};

#endif