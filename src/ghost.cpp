#include <Ghost.hpp>
#include <engine.hpp>
std::unique_ptr<Model> Ghost::_model;
std::unique_ptr<ImageTexture2D> Ghost::_material;
Ghost::Ghost(Engine *engine, glm::vec3 scale, glm::vec3 position, std::string _material_path, std::string _model_path) : Object(engine, Category::GHOST)
{
    engine->addObject(this);
    init(_engine->_assetRootDir + _material_path, _engine->_assetRootDir + _model_path);
    _transform.scale = scale;
    _transform.position = position;
}
void Ghost::init(std::string _material_path, std::string _model_path)
{
    if (!_is_loaded)
    {
        Ghost::_model.reset(new Model(_model_path));
        Ghost::_material.reset(new ImageTexture2D(_material_path));
        _is_loaded = true;
    }
}
void Ghost::plot()
{
    normalizingShader(_shader_index, &(_transform));
    auto shader = _engine->_shaders[_shader_index].get();
    shader->use();
    _material->bind(0);
    shader->setUniformInt("mapKd", 0);
    shader->setUniformVec3("material.ks", glm::vec3(0, 0, 0));
    shader->setUniformVec3("material.ka", glm::vec3(0.2, 0.2, 0.2));
    shader->setUniformFloat("material.ns", 100.0f);
    shader->setUniformVec3("scale", glm::vec3(1.5f, 1.5f, 1.5f));
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
void Ghost::renew()
{
    glm::vec3 _shooter_pos = _engine->shooter->_transform.position;
    float _deltaX = _transform.position.x - _shooter_pos.x;
    float _deltaZ = _transform.position.z - _shooter_pos.z;
    if (sqrt(_deltaX * _deltaX + _deltaZ * _deltaZ) <= _range_radius)
    {
        _move_dir = _shooter_pos - _transform.position;
    }
    else
    {
        _move_dir = glm::vec3(0.0f);
    }
    _transform.position += _engine->_deltaTime * _speed * _move_dir;
}