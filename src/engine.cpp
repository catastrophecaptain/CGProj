#include <engine.hpp>
#include <object.hpp>
#include <command.hpp>
#include "GLFW/glfw3.h"
#include <iostream>
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
void Engine::start() {

};
void Engine::getcommand() {

};
void Engine::renew()
{
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
        if (object->isfixed())
        {
            for (auto object_move : _objects_move)
            {
                if (checkcollision(object, object_move))
                {
                    object->collidedby(object_move);
                    object_move->collidedby(object);
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
bool Engine::checkcollision(Object *fixed, Object *move)
{
    return false;
};
void Engine::handleInput()
{
    getcommand();
    renew();
    check();
};
void Engine::renderFrame()
{
    plot();
};
