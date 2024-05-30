#pragma once
#include "base/application.h"
#include "base/camera.h"
#include "base/light.h"
#include "base/glsl_program.h"
#include "shooter.hpp"
#include "ghost.hpp"
#include "bullet.hpp"
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
    int addShader(const std::string &vertexShader, const std::string &fragmentShader);
    void initShaders();
    void initlights();
    std::vector<Object *> _objects;
    std::vector<Object *> _objects_move;
    std::vector<std::unique_ptr<GLSLProgram>> _shaders;
    EngineStage _stage;
    std::unique_ptr<PerspectiveCamera> _camera;
    std::unique_ptr<DirectionalLight> _light_directional;
    std::unique_ptr<PointLight> _light_point;
    std::unique_ptr<AmbientLight> _light_ambient;
    Command *_command = nullptr;
    // shooter和ghost
    Shooter* shooter;
    // std::vector<Ghost*> _ghosts;
    std::vector<Bullet*> _bullets;
    friend class Object;
    friend class Command;
    friend class Ambience;
    friend class Shooter;
    friend class Ghost;
    friend class Map;

    // 只是为了测试
    void cameraRenew();
};