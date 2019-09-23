#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define MAX_VERTEX_COUNT 100000
#define MAX_VERTEX_SIZE 24
#define MAX_IMAGE_DIMENSION 1024
#define MAX_TEXTURE_COUNT 10
#define MAX_UNIFORM_COUNT 15

enum DrawableType
{
    NotTextured,
    Textured,
    TexturedWithNormalMap,
    Count
};

#endif