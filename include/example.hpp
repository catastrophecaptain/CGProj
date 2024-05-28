#include <object.hpp>
#include <base/model.h>
#include <memory>
#include <base/glsl_program.h>
#include <base/texture.h>
#include <base/texture2d.h>
class Example : public Object
{
    public:
    Example(Engine *engine);
    void init();
    void plot() override;
    std::unique_ptr<Model> _model;
    std::unique_ptr<GLSLProgram> _Shader;
    std::unique_ptr<ImageTexture2D> _Material;
};