#pragma once
#include <object.hpp>
#include <engine.hpp>

#include <base/glsl_program.h>
#include <base/texture_cubemap.h>
class Ambience : public Object
{
public:
    Ambience(Engine *engine);
    void init(const std::vector<std::string> &textureFilenames);

    Ambience(Ambience &&rhs) noexcept;

    ~Ambience();

    void draw(const glm::mat4 &projection, const glm::mat4 &view);
    void plot() override;
    GLuint _vao = 0;
    GLuint _vbo = 0;

    std::unique_ptr<TextureCubemap> _texture;

    std::unique_ptr<GLSLProgram> _shader;
    void cleanup();
};