#ifndef SHOOTER_HPP
#define SHOOTER_HPP

#include <object.hpp>
#include <base/model.h>
#include <memory>
#include <base/glsl_program.h>
#include <base/texture.h>
#include <base/texture2d.h>
#include "engine.hpp"
class Shooter : public Object
{
public:
    Shooter(Engine *engine, glm::vec3 scale, glm::vec3 position, glm::quat rotation,
            std::vector<std::string> _material_path = {"texture/shooter/armColor.png", "texture/shooter/persuadercolor.png"}, std::vector<std::string> _model_path = {"obj/shooter/arm", "obj/shooter/gun"});
    void init(std::vector<std::string> _material_path, std::vector<std::string> _model_path);
    void plot() override;
    void frame_shoot_control();
    void change_stage(EngineStage stage);
    // 当射击的时候调用frame_shoot,会完成射击动作
    void frame_shoot();
    void renew() override;
    void renew_camera();
    std::vector<Box> getBoxs() override;
    std::vector<Segment> getSegments() override;
    void collidedBy(Object *other) override;
    Transform _transform;
    int _model_current_index = 0;
    int _frame_cnt = 10;
    std::vector<std::unique_ptr<Model>> _model;
    static const int _shader_index = 0;
    std::vector<std::unique_ptr<ImageTexture2D>> _material;
    bool _is_shooting = false;
    Transform _model_transform;
    Transform _transform_old;
    BoundingBox _bounding_box;
    bool _is_end = false;
    bool _is_view_mode = true;
    float _gravity = 1600;
    float _speed = 100.0;
    float _angle_speed = 0.02;
    float _fovy_init= glm::radians(50.0f);
    float _up_speed = 600.0;
    float _up_speed_once = 600.0;

};

#endif // SHOOTER_HPP