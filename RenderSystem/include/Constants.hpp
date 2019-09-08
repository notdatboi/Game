#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

enum Semaphores{ImageAcquired1, ImageRendered1, ImageAcquired2, ImageRendered2, ImageAcquired3, ImageRendered3, Count};
enum Fences{ImageAcquired1, ImageRendered1, ImageAcquired2, ImageRendered2, ImageAcquired3, ImageRendered3, Count};
enum MemoryObjects{Transfer, VertexIndex, Uniform, Image, Count};
enum Buffers{Transfer, VertexIndex, Uniform, Count};
enum Images{DepthAttachment1, DepthAttachment2, DepthAttachment3, Count};
enum Pipelines{Count};
enum PipelineLayouts{Count};
enum DescriptorSetLayouts{Count};
enum DescriptorSets{Count};

#endif