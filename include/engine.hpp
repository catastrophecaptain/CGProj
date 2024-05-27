#pragma once
#include <vector>
#include "base/application.h"
class Object;
class Command;
enum class EngineStage{
    HALT,
    START,
    RUN,
    END
};

class Engine :public Application
{
public:
    Engine(const Options& options);
    ~Engine();
    void start();
    void getcommand();
    void renew();
    void check();
    void plot();
    // 对齐接口
    virtual void handleInput();
    virtual void renderFrame();
    bool checkcollision(Object *fixed,Object *move);
    std::vector<Object *> _objects;
    std::vector<Object *> _objects_move;
    Command * _command;
};