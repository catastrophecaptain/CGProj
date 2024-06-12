#include <ghost.hpp>
#include <engine.hpp>
#include <shooter.hpp>
std::unique_ptr<Model> Ghost::_model;
std::unique_ptr<ImageTexture2D> Ghost::_material;
float Ghost::_speed=0.1f;
int Ghost::_dead=0;
Ghost::Ghost(Engine *engine, glm::vec3 scale, glm::vec3 position, bool is_add, std::string _material_path, std::string _model_path) : Object(engine, Category::GHOST)
{
    if (is_add)
    {
        engine->addObject(this,true);
    }
    init(_engine->_assetRootDir + _material_path, _engine->_assetRootDir + _model_path);
    _transform.scale = scale;
    _transform.position = position;
    _transform_old=_transform;
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
    _transform_old = _transform;
    glm::vec3 _shooter_pos = _engine->shooter->_transform.position;
    float _deltaX = _transform.position.x - _shooter_pos.x;
    float _deltaZ = _transform.position.z - _shooter_pos.z;
    _transform.lookAt(_shooter_pos);
    _transform.rotation = glm::rotate(_transform.rotation, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if (_engine->_stage == EngineStage::RUN)
    {
        if (1)//(sqrt(_deltaX * _deltaX + _deltaZ * _deltaZ) <= _range_radius)
        {
            _move_dir = _shooter_pos - _transform.position;
        }
        else
        {
            _move_dir = glm::vec3(0.0f);
        }
        _transform.position += _engine->_deltaTime * _speed * _move_dir;
    }
    _speed=(_speed+0.0001<0.4)?_speed+0.0001:0.4;
}
std::vector<Box> Ghost::getBoxs()
{
    BoundingBox box = _model->getBoundingBox();
    std::vector<Box> boxs;
    boxs.push_back(Box(box.min, box.max, _transform));
    return boxs;
}
std::vector<Segment> Ghost::getSegments()
{
    BoundingBox box = _model->getBoundingBox();
    std::vector<Segment> segments;
    glm::vec3 vertex_1[20];
    Transform _transform_1{_transform};
    Transform _transform_old_1{_transform_old};
    vertex_1[0] = _transform_1.getLocalMatrix() * glm::vec4(box.min, 1.0f);
    vertex_1[1] = _transform_1.getLocalMatrix() * glm::vec4(box.max, 1.0f);
    vertex_1[2] = _transform_1.getLocalMatrix() * glm::vec4(box.min.x, box.min.y, box.max.z, 1.0f);
    vertex_1[3] = _transform_1.getLocalMatrix() * glm::vec4(box.min.x, box.max.y, box.min.z, 1.0f);
    vertex_1[4] = _transform_1.getLocalMatrix() * glm::vec4(box.min.x, box.max.y, box.max.z, 1.0f);
    vertex_1[5] = _transform_1.getLocalMatrix() * glm::vec4(box.max.x, box.min.y, box.min.z, 1.0f);
    vertex_1[6] = _transform_1.getLocalMatrix() * glm::vec4(box.max.x, box.min.y, box.max.z, 1.0f);
    vertex_1[7] = _transform_1.getLocalMatrix() * glm::vec4(box.max.x, box.max.y, box.min.z, 1.0f);
    vertex_1[8] = (vertex_1[0] + vertex_1[2]) / 2.0f;
    vertex_1[9] = (vertex_1[0] + vertex_1[3]) / 2.0f;
    vertex_1[10] = (vertex_1[0] + vertex_1[5]) / 2.0f;
    vertex_1[11] = (vertex_1[1] + vertex_1[4]) / 2.0f;
    vertex_1[12] = (vertex_1[1] + vertex_1[6]) / 2.0f;
    vertex_1[13] = (vertex_1[1] + vertex_1[7]) / 2.0f;
    vertex_1[14] = (vertex_1[1] + vertex_1[8]) / 2.0f;
    glm::vec3 vertex_2[20];
    vertex_2[0] = _transform_old_1.getLocalMatrix() * glm::vec4(box.min, 1.0f);
    vertex_2[1] = _transform_old_1.getLocalMatrix() * glm::vec4(box.max, 1.0f);
    vertex_2[2] = _transform_old_1.getLocalMatrix() * glm::vec4(box.min.x, box.min.y, box.max.z, 1.0f);
    vertex_2[3] = _transform_old_1.getLocalMatrix() * glm::vec4(box.min.x, box.max.y, box.min.z, 1.0f);
    vertex_2[4] = _transform_old_1.getLocalMatrix() * glm::vec4(box.min.x, box.max.y, box.max.z, 1.0f);
    vertex_2[5] = _transform_old_1.getLocalMatrix() * glm::vec4(box.max.x, box.min.y, box.min.z, 1.0f);
    vertex_2[6] = _transform_old_1.getLocalMatrix() * glm::vec4(box.max.x, box.min.y, box.max.z, 1.0f);
    vertex_2[7] = _transform_old_1.getLocalMatrix() * glm::vec4(box.max.x, box.max.y, box.min.z, 1.0f);
    vertex_2[8] = (vertex_2[0] + vertex_2[2]) / 2.0f;
    vertex_2[9] = (vertex_2[0] + vertex_2[3]) / 2.0f;
    vertex_2[10] = (vertex_2[0] + vertex_2[5]) / 2.0f;
    vertex_2[11] = (vertex_2[1] + vertex_2[4]) / 2.0f;
    vertex_2[12] = (vertex_2[1] + vertex_2[6]) / 2.0f;
    vertex_2[13] = (vertex_2[1] + vertex_2[7]) / 2.0f;
    vertex_2[14] = (vertex_2[1] + vertex_2[8]) / 2.0f;
    for (int i = 0; i < 14; i++)
    {
        segments.push_back(Segment{vertex_1[i], vertex_2[i]});
    }
    segments.push_back(Segment{_transform_1.position,_transform_old.position});
    return segments;
}
void Ghost::collidedBy(Object *other)
{
    // int x = _num_generator(gen);
    //     int y = _num_generator(gen);
    //     int z = _num_generator(gen);
    //     float ghost_scale = 6.0f;
    //     glm::vec3 scale(ghost_scale, ghost_scale, ghost_scale);
    //     glm::vec3 position((x - 250.0f) * 0.8f, fmod(y, 40.0f) + 20.0f, (z - 250.0f) * 2.5f);
    //     Ghost *ghost = new Ghost(this, scale, position);
    if (other->getCategory() == Category::BULLET)
    {
        // if (!_is_to_delete)
        // {
            // _engine->_objects_to_delete.push_back(this);
            // _is_to_delete = true;
            // int cnt = 1;
            // std::srand(std::time(0));
            // if (std::rand() % 100 < 10)
            // {
            //     cnt = 2;
            // }
            // std::cout << "new ghost" << std::endl;
            // for (int i = 0; i < cnt; i++)
            // {
            //     int x = rand() % 500;
            //     int y = rand() % 500;
            //     int z = rand() % 500;
            //     float ghost_scale = 6.0f;
            //     glm::vec3 scale(ghost_scale, ghost_scale, ghost_scale);
            //     glm::vec3 position((x - 250.0f) * 0.8f, fmod(y, 40.0f) + 20.0f, (z - 250.0f) * 2.5f);
            //     Ghost *ghost = new Ghost(_engine, scale, position, false);
            //     _engine->_objects_to_add.push_back(ghost);
            // }
            // _is_to_delete = true;
            std::srand(std::time(0)+_dead);
            _dead++;
            int x = rand() % 500;
            int y = rand() % 500;
            int z = rand() % 500;
            glm::vec3 position((x - 250.0f) * 0.8f, fmod(y, 40.0f) + 80.0f, (z - 250.0f) * 2.5f);
            _engine->killCount++;
            _transform.position = position;
        // }
    }
}