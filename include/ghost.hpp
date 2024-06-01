#ifndef GHOST_HPP
#define GHOST_HPP

#include <object.hpp>
#include <base/model.h>
#include <memory>
#include <base/glsl_program.h>
#include <base/texture.h>
#include <base/texture2d.h>
class Ghost : public Object
{
public:
    Ghost(Engine *engine, glm::vec3 scale, glm::vec3 position,bool is_add=true, std::string _material_path = "texture/ghost/T_Ghost.png", std::string _model_path = "obj/ghost/ghost.obj");
    void init(std::string _material_path, std::string _model_path);
    void plot() override;
    void renew() override;
    std::vector<Box> getBoxs() override;
    std::vector<Segment> getSegments() override;
    void collidedBy(Object *other) override;
    static std::unique_ptr<Model> _model;
    static const int _shader_index = 0;
    static std::unique_ptr<ImageTexture2D> _material;
    // bool _is_loaded = false;
    static float _speed ;
    glm::vec3 _move_dir = glm::vec3(0.0f);
    Transform _transform;
    bool _is_loaded = false;
    bool _is_to_delete = false;
    float _range_radius = 100.0f; // 如果shooter走进这个范围内，ghost就开始向shooter移动
    static int _dead;
};

#endif // GHOST_HPP