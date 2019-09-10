#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

enum Semaphores{ImageAcquired1, ImageRendered1, ImageAcquired2, ImageRendered2, ImageAcquired3, ImageRendered3, TransferFilished, Count};
enum Fences{ImageAcquired1, ImageRendered1, ImageAcquired2, ImageRendered2, ImageAcquired3, ImageRendered3, TransferSubmitted, Count};
enum MemoryObjects{Transfer, VertexIndex, Uniform, Image, DepthAttachment, Count};
enum Buffers{Transfer, VertexIndex, Uniform, Count};
enum DepthImages{DepthAttachment1, DepthAttachment2, DepthAttachment3, Count};
enum CommandBuffers{Render1, Render2, Render3, Transfer, Count};
enum Pipelines{Count};
enum PipelineLayouts{Count};
enum DescriptorSetLayouts{Count};
enum DescriptorSets{Count};

#endif