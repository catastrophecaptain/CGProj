#pragma once
#include "imgui_impl_glfw.h"
#include "base/camera.h"
#include "engine.hpp"
#include "ghost.hpp"
#include "base/transform.h"
#include <random>
#include <vector>
#include <iostream>
class Command
{
public:
    Command(Engine* engine);
    ~Command();
    void handleInput();
    void generateGhost();
    int _view = 0;
private:
    std::vector<std::unique_ptr<PerspectiveCamera>> _cameras;
    std::vector<Transform> transforms;
    Engine* _engine;
};