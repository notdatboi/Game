LIBS= -lvulkan -lglfw -lassimp
CC=g++ -std=c++17 -I./include -I./include/Glslang
BIN=a.out
SOURCES=$(wildcard src/*.cpp)
OBJS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES))

all: $(OBJS)

obj/System.o: src/System.cpp \
	include/Utils.hpp \
	include/Window.hpp 
	$(CC) -c $< -o $@ -g

obj/MemoryPool.o: src/MemoryPool.cpp \
	include/MemoryPool.hpp \
	include/Utils.hpp \
	include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/Swapchain.o: src/Swapchain.cpp \
	include/Swapchain.hpp \
	include/System.hpp 
	$(CC) -c $< -o $@ -g

obj/Utils.o: src/Utils.cpp \
	include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/Window.o: src/Window.cpp \
	include/Window.hpp \
	include/Utils.hpp 
	$(CC) -c $< -o $@ -g

obj/main.o: src/main.cpp \
	include/System.hpp \
	include/Utils.hpp \
	include/Swapchain.hpp \
	include/MemoryPool.hpp \
	include/Window.hpp
	$(CC) -c $< -o $@ -g