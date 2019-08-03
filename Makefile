LIBS= -lvulkan -lglfw -lassimp
CC=g++ -std=c++17 -I./include -I./include/Glslang
BIN=a.out
SOURCES=$(wildcard src/*.cpp)
OBJS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES))

all: $(OBJS)

obj/Executives.o: src/Executives.cpp \
	include/Executives.hpp \
	include/SparkIncludeBase.hpp \
	include/System.hpp
	$(CC) -c $< -o $@ -g

obj/MemoryManager.o: src/MemoryManager.cpp \
	include/MemoryManager.hpp \
	include/SparkIncludeBase.hpp \
	include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/System.o: src/System.cpp \
	include/System.hpp \
	include/Executives.hpp \
	include/SparkIncludeBase.hpp
	$(CC) -c $< -o $@ -g

obj/ShaderSet.o: src/ShaderSet.cpp \
	include/Shader.hpp \
	include/ShaderSet.hpp \
	include/System.hpp \
	include/SparkIncludeBase.hpp
	$(CC) -c $< -o $@ -g

obj/Shader.o: src/Shader.cpp \
	include/Glslang/DirStackFileIncluder.h \
	include/Shader.hpp \
	include/System.hpp \
	include/SparkIncludeBase.hpp
	$(CC) -c $< -o $@ -g

obj/Image.o: src/Image.cpp \
	include/Image.hpp \
	include/System.hpp \
	include/Executives.hpp \
	include/MemoryManager.hpp \
	include/SparkIncludeBase.hpp
	$(CC) -c $< -o $@ -g

obj/ImageView.o: src/ImageView.cpp \
	include/ImageView.hpp \
	include/System.hpp \
	include/SparkIncludeBase.hpp
	$(CC) -c $< -o $@ -g

obj/Buffer.o: src/Buffer.cpp \
	include/Buffer.hpp \
	include/System.hpp \
	include/Executives.hpp \
	include/MemoryManager.hpp \
	include/SparkIncludeBase.hpp
	$(CC) -c $< -o $@ -g

obj/Subpass.o: src/Subpass.cpp \
	include/Subpass.hpp \
	include/SparkIncludeBase.hpp \
	include/Executives.hpp \
	include/System.hpp
	$(CC) -c $< -o $@ -g

obj/RenderPass.o: src/RenderPass.cpp \
	include/RenderPass.hpp \
	include/SparkIncludeBase.hpp \
	include/Executives.hpp \
	include/System.hpp
	$(CC) -c $< -o $@ -g

obj/Pipeline.o: src/Pipeline.cpp \
	include/Pipeline.hpp \
	include/SparkIncludeBase.hpp \
	include/System.hpp
	$(CC) -c $< -o $@ -g

obj/DescriptorPool.o: src/DescriptorPool.cpp \
	include/DescriptorPool.hpp \
	include/SparkIncludeBase.hpp \
	include/System.hpp
	$(CC) -c $< -o $@ -g

obj/Swapchain.o: src/Swapchain.cpp \
	include/Swapchain.hpp \
	include/SparkIncludeBase.hpp \
	include/System.hpp \
	include/Executives.hpp \
	include/ImageView.hpp
	$(CC) -c $< -o $@ -g