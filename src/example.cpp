#include <example.hpp>
#include <engine.hpp>
#include <base/glsl_program.h>
#include <base/texture.h>

Example::Example(Engine *engine) : Object(engine, Category::ENEMY)
{
    engine->addObject(this);
    const char *vsCode =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPosition;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "layout(location = 2) in vec2 aTexCoord;\n"
        "out vec2 fTexCoord;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"

        "void main() {\n"
        "    fTexCoord = aTexCoord;\n"
        "    gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
        "}\n";

    const char *fsCode =
        "#version 330 core\n"
        "in vec2 fTexCoord;\n"
        "out vec4 color;\n"
        "uniform sampler2D mapKd;\n"
        "void main() {\n"
        "    color = texture(mapKd, fTexCoord);\n"
        "}\n";

    _Shader.reset(new GLSLProgram);
    _Shader->attachVertexShader(vsCode);
    _Shader->attachFragmentShader(fsCode);
    _Shader->link();
    init();
};
void Example::init()
{
    _Material.reset(new ImageTexture2D("../media/textures/arm1Color.png"));
    _model.reset(new Model("../media/3.obj"));
};
void Example::plot()
{
    const glm::mat4 projection = _engine->_camera->getProjectionMatrix();
    const glm::mat4 view = _engine->_camera->getViewMatrix();
    _Shader->use();
    // 2. transfer mvp matrices to gpu
    _Shader->setUniformMat4("projection", projection);
    _Shader->setUniformMat4("view", view);
    _Shader->setUniformMat4("model", _model->transform.getLocalMatrix());
    // 3. enable textures and transform textures to gpu
    // _simpleMaterial->mapKd->bind();
    _Material->bind();
    _model->draw();
};