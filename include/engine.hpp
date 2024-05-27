#pragma once
#include "base/application.h"
#include "base/camera.h"
#include "base/light.h"

#include <vector>
#include <memory>
class Object;
class Command;
enum class EngineStage
{
    HALT,
    START,
    RUN,
    END
};

class Engine : public Application
{
public:
    // 同时构造了Application,并且调用了start
    Engine(const Options &options);
    ~Engine();
    void start();
    void getCommand();
    void renew();
    void check();
    void plot();
    void addObject(Object *object, bool move = false);
    void deleteObject(Object *object);
    // 对齐接口
    virtual void handleInput() override;
    virtual void renderFrame() override;
    bool checkCollision(Object *fixed, Object *move);
    std::vector<Object *> _objects;
    std::vector<Object *> _objects_move;
    EngineStage _stage;
    std::unique_ptr<PerspectiveCamera> _camera;
    std::unique_ptr<DirectionalLight> _light_directional;
    std::unique_ptr<PointLight> _light_point;
    Command *_command;
    friend class Object;
    friend class Command;
    friend class Ambience;

    // 只是为了测试
    void cameraRenew();
};