#include <Ghost.hpp>
#include <engine.hpp>
Ghost::Ghost(Engine *engine, std::string _material_path, std::string _model_path) : Object(engine, Category::GHOST)
{
    engine->addObject(this);
    init(_engine->_assetRootDir + _material_path, _engine->_assetRootDir + _model_path);
}
void Ghost::init(std::string _material_path, std::string _model_path)
{
    _model.reset(new Model(_model_path));
    _material.reset(new ImageTexture2D(_material_path));
}
void Ghost::plot()
{
    normalizingShader(_shader_index,&(_model->transform));
    auto shader = _engine->_shaders[_shader_index].get();
    shader->use();
    _material->bind(0);
    shader->setUniformInt("mapKd", 0);
    shader->setUniformVec3("material.ks", glm::vec3(0, 0, 0));
    shader->setUniformVec3("material.ka", glm::vec3(0.2, 0.2, 0.2));
    shader->setUniformFloat("material.ns", 100.0f);
    // shader->setUniformFloat("material.ns", 100.0f);

    // auto _Shader = _engine->_shaders[0].get();
    // const glm::mat4 projection = _engine->_camera->getProjectionMatrix();
    // const glm::mat4 view = _engine->_camera->getViewMatrix();
    // _Shader->use();
    // // 2. transfer mvp matrices to gpu
    // _Shader->setUniformMat4("projection", projection);
    // _Shader->setUniformMat4("view", view);
    // _Shader->setUniformMat4("model", _model->transform.getLocalMatrix());
    _model->draw();
}