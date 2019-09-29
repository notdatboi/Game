LIBS= -lvulkan -lglfw -lassimp -lglslang -lSPIRV
CC=g++ -std=c++17 -IRenderSystem/include/ -IRenderSystem/include/External/Glslang/
BIN=a.out
SOURCES=$(wildcard RenderSystem/src/*.cpp)
OBJS=$(patsubst RenderSystem/src/%.cpp,obj/%.o,$(SOURCES)) obj/main.o
#RSI=RenderSystem/include/
#RSS=RenderSystem/src/

all: $(BIN)

clean:
	rm obj/* && make all

$(BIN): $(OBJS)
	$(CC) $(OBJS) $(LIBS)

obj/Utils.o: RenderSystem/src/Utils.cpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/System.o: RenderSystem/src/System.cpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/Window.hpp 
	$(CC) -c $< -o $@ -g

obj/BufferHolder.o: RenderSystem/src/BufferHolder.cpp \
	RenderSystem/include/BufferHolder.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/CommandPool.o: RenderSystem/src/CommandPool.cpp \
	RenderSystem/include/CommandPool.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/DescriptorLayoutHolder.o: RenderSystem/src/DescriptorLayoutHolder.cpp \
	RenderSystem/include/DescriptorLayoutHolder.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/DescriptorPool.o: RenderSystem/src/DescriptorPool.cpp \
	RenderSystem/include/DescriptorPool.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/GraphicsPipelineUtils.o: RenderSystem/src/GraphicsPipelineUtils.cpp \
	RenderSystem/include/GraphicsPipelineUtils.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/ImageHolder.o: RenderSystem/src/ImageHolder.cpp \
	RenderSystem/include/ImageHolder.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/ImageLoader.o: RenderSystem/src/ImageLoader.cpp \
	RenderSystem/include/ImageLoader.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/Material.o: RenderSystem/src/Material.cpp \
	RenderSystem/include/Material.hpp \
	RenderSystem/include/Constants.hpp \
	RenderSystem/include/ImageHolder.hpp \
	RenderSystem/include/ImageLoader.hpp \
	RenderSystem/include/ObjectManagementStrategy.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/MemoryPool.o: RenderSystem/src/MemoryPool.cpp \
	RenderSystem/include/MemoryPool.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/Mesh.o: RenderSystem/src/Mesh.cpp \
	RenderSystem/include/Mesh.hpp \
	RenderSystem/include/Material.hpp \
	RenderSystem/include/BufferHolder.hpp \
	RenderSystem/include/MeshUtils.hpp \
	RenderSystem/include/ObjectManagementStrategy.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/MeshUtils.o: RenderSystem/src/MeshUtils.cpp \
	RenderSystem/include/MeshUtils.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/ObjectManagementStrategy.o: RenderSystem/src/ObjectManagementStrategy.cpp \
	RenderSystem/include/ObjectManagementStrategy.hpp \
	RenderSystem/include/Constants.hpp \
	RenderSystem/include/BufferHolder.hpp \
	RenderSystem/include/ImageHolder.hpp \
	RenderSystem/include/ImageLoader.hpp \
	RenderSystem/include/CommandPool.hpp \
	RenderSystem/include/DescriptorLayoutHolder.hpp \
	RenderSystem/include/DescriptorPool.hpp \
	RenderSystem/include/MemoryPool.hpp \
	RenderSystem/include/SynchronizationPool.hpp \
	RenderSystem/include/Utils.hpp \
	RenderSystem/include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/Renderer.o: RenderSystem/src/Renderer.cpp \
	RenderSystem/include/System.hpp \
	RenderSystem/include/Swapchain.hpp \
	RenderSystem/include/RenderPassHolder.hpp \
	RenderSystem/include/SynchronizationPool.hpp \
	RenderSystem/include/ObjectManagementStrategy.hpp \
	RenderSystem/include/Scene.hpp \
	RenderSystem/include/GraphicsPipelineUtils.hpp \
	RenderSystem/include/Shader.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/RenderPassHolder.o: RenderSystem/src/RenderPassHolder.cpp \
	RenderSystem/include/RenderPassHolder.hpp \
	RenderSystem/include/System.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/Scene.o: RenderSystem/src/Scene.cpp \
	RenderSystem/include/Scene.hpp \
	RenderSystem/include/System.hpp \
	RenderSystem/include/Mesh.hpp \
	RenderSystem/include/Material.hpp \
	RenderSystem/include/ObjectManagementStrategy.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/Shader.o: RenderSystem/src/Shader.cpp \
	RenderSystem/include/Shader.hpp \
	RenderSystem/include/System.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/Swapchain.o: RenderSystem/src/Swapchain.cpp \
	RenderSystem/include/Swapchain.hpp \
	RenderSystem/include/System.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/SynchronizationPool.o: RenderSystem/src/SynchronizationPool.cpp \
	RenderSystem/include/SynchronizationPool.hpp \
	RenderSystem/include/System.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/Window.o: RenderSystem/src/Window.cpp \
	RenderSystem/include/Window.hpp \
	RenderSystem/include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/main.o: main.cpp \
	RenderSystem/include/Renderer.hpp \
	RenderSystem/include/Window.hpp 
	$(CC) -c $< -o $@ -g