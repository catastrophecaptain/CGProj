#include "command.hpp"
#include "engine.hpp"
#include "shooter.hpp"
Command::Command(Engine *engine) : _engine(engine){

                                   };

Command::~Command() = default;

void Command::handleInput()
{
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE)
    {
        glfwSetWindowShouldClose(_engine->_window, true);
        return;
    }
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_SPACE] != GLFW_RELEASE)
    {
        std::cout << "SPACE" << std::endl;
        if (_engine->_stage == EngineStage::START)
        {
            _engine->_stage = EngineStage::RUN;
        }
    }
    if (_stage_before != _engine->_stage)
    {
        _stage_before = _engine->_stage;
        _engine->shooter->change_stage(_engine->_stage);
    }
}