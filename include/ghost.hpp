#include <object.hpp>
#include <base/model.h>
#include <memory>
#include <base/glsl_program.h>
#include <base/texture.h>
#include <base/texture2d.h>
class Ghost : public Object
{
    public:
    Ghost(Engine *engine,std::string _material_path="texture/ghost/T_Ghost.png",std::string _model_path="obj/ghost/ghost.obj");
    void init(std::string _material_path,std::string _model_path);
    void plot() override;
    std::unique_ptr<Model> _model;
    static const int _shader_index=0;
    std::unique_ptr<ImageTexture2D> _material;
    // bool _is_loaded = false;
};