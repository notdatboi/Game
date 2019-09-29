#include<Renderer.hpp>
#include<chrono>
#include<iostream>

int main()
{
    glfwInit();
    Window window;
    VkExtent2D windowExtent = {1366, 768};
    window.create("TEST", windowExtent);
    Renderer renderer;
    std::vector<std::string> scenes = {"Models/WoodenCup.dae"};
    std::string imagePath = "Models/";
    renderer.create(window, scenes, imagePath);
    std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
    float fps = 1 / 5.0f;
    float passedTime = 0;
    //const auto& map = renderer.getScene(0).getRootNode().getChildrenNodes();
    //std::cout << (*map.at("Cylinder").getChildrenNodes().begin()).first << "<-Size\n";
    //for(const auto& kv : map) std::cout << kv.first << '\n';
    while (!glfwWindowShouldClose(window.getWindow()))
    {
        glfwPollEvents();
        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        std::chrono::duration<float> duration = end - begin;
        passedTime += duration.count();
        //std::cout << passedTime << '\n';
        begin = end;
        if(passedTime > fps)
        {
            renderer.beginRendering();
            renderer.renderSceneNode(renderer.getScene(0)["Cylinder"]);
            renderer.endRendering();
            passedTime -= fps;
        }
    }
    renderer.destroy();
    window.destroy();
    glfwTerminate();
}