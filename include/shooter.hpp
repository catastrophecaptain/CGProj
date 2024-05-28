#include <object.hpp>
#include <base/model.h>
#include <memory>
#include <base/glsl_program.h>
#include <base/texture.h>
#include <base/texture2d.h>
class Shooter : public Object
{
    public:
    Shooter(Engine *engine,std::string _material_path="texture/shooter/arm1Color.png",std::string _model_path="obj/shooter/1.obj");
    void init(std::string _material_path,std::string _model_path);
    void plot() override;
    std::unique_ptr<Model> _model;
    static const int _shader_index=0;
    std::unique_ptr<ImageTexture2D> _material;
    // bool _is_loaded = false;
};