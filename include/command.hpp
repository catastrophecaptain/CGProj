#pragma once
#include "imgui_impl_glfw.h"
#include "base/camera.h"
#include "base/transform.h"
#include <random>
#include <vector>
#include <iostream>
#include "engine.hpp"
class Command
{
public:
    Command(Engine* engine);
    ~Command();
    void handleInput();
    EngineStage _stage_before=EngineStage::START;
    Engine* _engine;
};