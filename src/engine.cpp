#include <engine.hpp>
#include <object.hpp>
#include <command.hpp>
#include <iostream>
#include "ambience.hpp"
Engine::Engine(const Options &options) : Application(options)
{
    start();
};
Engine::~Engine()
{
    for (auto object : _objects)
    {
        delete object;
    }
    for (auto object : _objects_move)
    {
        delete object;
    }
    delete _command;
};
void Engine::start()
{
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
    _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
    glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
    _camera.reset(new PerspectiveCamera(
        glm::radians(50.0f), 1.0f * _windowWidth / _windowHeight, 0.1f, 1000.0f));
    _camera->transform.position.z = 10.0f;
    Ambience *ambience = new Ambience(this);
};
void Engine::getCommand()
{
    _input.forwardState();
};
void Engine::renew()
{
    cameraRenew();
    for (auto object : _objects)
    {
        object->renew();
    }
    for (auto object : _objects_move)
    {
        object->renew();
    }
};
void Engine::check()
{
    for (auto object : _objects)
    {
        if (object->isFixed())
        {
            for (auto object_move : _objects_move)
            {
                if (checkCollision(object, object_move))
                {
                    object->collidedBy(object_move);
                    object_move->collidedBy(object);
                }
            }
        }
    }
};
void Engine::plot()
{
    for (auto object : _objects)
    {
        object->plot();
    }
};
bool Engine::checkCollision(Object *fixed, Object *move)
{
    return false;
};
void Engine::handleInput()
{
    renew();
    check();
    getCommand();
};
void Engine::renderFrame()
{
    showFpsInWindowTitle();
    plot();
};
void Engine::addObject(Object *object, bool move)
{
    if (move)
    {
        _objects_move.push_back(object);
        _objects.push_back(object);
    }
    else
    {
        _objects.push_back(object);
    }
};
void Engine::deleteObject(Object *object)
{
    for (auto it = _objects.begin(); it != _objects.end(); it++)
    {
        if (*it == object)
        {
            _objects.erase(it);
            break;
        }
    }
    for (auto it = _objects_move.begin(); it != _objects_move.end(); it++)
    {
        if (*it == object)
        {
            _objects_move.erase(it);
            break;
        }
    }
};
void Engine::cameraRenew()
{
    constexpr float cameraMoveSpeed = 5.0f;
    constexpr float cameraRotateSpeed = 0.02f;

    if (_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE)
    {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE)
    {
        std::cout << "W" << std::endl;
        _camera->transform.position += _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE)
    {
        std::cout << "A" << std::endl;
        _camera->transform.position -= _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE)
    {
        std::cout << "S" << std::endl;
        _camera->transform.position -= _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE)
    {
        std::cout << "D" << std::endl;
        _camera->transform.position += _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
    }

    if (_input.mouse.move.xNow != _input.mouse.move.xOld)
    {
        std::cout << "mouse move in x direction" << std::endl;
        _camera->transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.xNow + _input.mouse.move.xOld)), glm::vec3(0.0f, 1.0f, 0.0f)) * _camera->transform.rotation;
    }

    if (_input.mouse.move.yNow != _input.mouse.move.yOld)
    {
        std::cout << "mouse move in y direction" << std::endl;
        _camera->transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.yNow + _input.mouse.move.yOld)), _camera->transform.getRight()) * _camera->transform.rotation;
    }
};