#pragma once
#include "base/application.h"
#include "base/camera.h"
#include "base/light.h"
#include "base/glsl_program.h"
#include <vector>
#include <memory>
#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment (lib,“WinMM.lib”)
class Bullet;
class Ghost;
class Shooter;
class Object;
class Command;
class Segment;
class Box;
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
    bool checkCollision(Box& box1, Segment& segment2);
    int addShader(const std::string &vertexShader, const std::string &fragmentShader);
    void initShaders();
    void initlights();
    std::vector<Object *> _objects;
    std::vector<Object *> _objects_move;
    std::vector<std::unique_ptr<GLSLProgram>> _shaders;
    EngineStage _stage=EngineStage::START;
    std::unique_ptr<PerspectiveCamera> _camera;
    std::unique_ptr<DirectionalLight> _light_directional;
    std::unique_ptr<PointLight> _light_point;
    std::unique_ptr<AmbientLight> _light_ambient;
    std::vector<Object *> _objects_to_delete;
    std::vector<Object *> _objects_to_add;
    Command *_command = nullptr;
    // shooter和ghost
    Shooter* shooter;
    // std::vector<Ghost*> _ghosts;
    std::vector<Bullet*> _bullets;
    float _t_min = 0.0;
    friend class Object;
    friend class Command;
    friend class Ambience;
    friend class Shooter;
    friend class Ghost;
    friend class Map;
    friend class Bullet;

    // 只是为了测试
};