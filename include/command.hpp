#pragma once
#include "imgui_impl_glfw.h"
#include "base/camera.h"
#include "engine.hpp"
#include <vector>
#include <iostream>
class Command
{
public:
    Command(Engine* engine);
    ~Command();
    void handleInput();
    void switchToRoam();
private:
    std::vector<std::unique_ptr<PerspectiveCamera>> _cameras;
    int _view = 0;
    std::vector<glm::vec3 > pos;
    Engine* _engine;
};