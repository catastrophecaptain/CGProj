#ifndef SHOOTER_HPP
#define SHOOTER_HPP

#include <object.hpp>
#include <base/model.h>
#include <memory>
#include <base/glsl_program.h>
#include <base/texture.h>
#include <base/texture2d.h>
class Shooter : public Object
{
public:
    Shooter(Engine *engine, std::vector<std::string> _material_path = {"texture/shooter/armColor.png", "texture/shooter/persuadercolor.png"}, std::vector<std::string> _model_path = {"obj/shooter/arm", "obj/shooter/gun"});
    void init(std::vector<std::string> _material_path, std::vector<std::string> _model_path);
    void plot() override;
    void frame_shoot_control();
    // 当射击的时候调用frame_shoot,会完成射击动作
    void frame_shoot();
    Transform _transform;
    int _model_current_index = 0;
    int _frame_cnt = 10;
    std::vector<std::unique_ptr<Model>> _model;
    static const int _shader_index = 0;
    std::vector<std::unique_ptr<ImageTexture2D>> _material;
    bool _is_shooting = false;
    // bool _is_loaded = false;
};

#endif // SHOOTER_HPP