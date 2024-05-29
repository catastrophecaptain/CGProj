#include <shooter.hpp>
#include <engine.hpp>
Shooter::Shooter(Engine *engine, std::vector<std::string> _material_path, std::vector<std::string> _model_path) : Object(engine, Category::SHOOTER)
{
    engine->addObject(this);
    init(_material_path, _model_path);
}
void Shooter::init(std::vector<std::string> _material_path, std::vector<std::string> _model_path)
{
    // 数据储存在形如"path%4d.obj"的文件中
    _model.resize(2 * _frame_cnt);
    _material.resize(2);
    for (int i = 0; i < 2; i++)
    {
        ImageTexture2D *temp = new ImageTexture2D(_engine->_assetRootDir + _material_path[i]);
        _material[i] = std::unique_ptr<ImageTexture2D>(temp);
    }
    for (int i = 0; i < _frame_cnt; i++)
    {
        std::string post_fix = std::to_string(i + 1);
        while (post_fix.size() < 4)
        {
            post_fix = "0" + post_fix;
        }
        _model[2 * i] = std::make_unique<Model>(_engine->_assetRootDir + _model_path[0] + post_fix + ".obj");
        _model[2 * i + 1] = std::make_unique<Model>(_engine->_assetRootDir + _model_path[1] + post_fix + ".obj");
    }
}
void Shooter::plot()
{
    normalizingShader(_shader_index, &_transform);
    auto shader = _engine->_shaders[_shader_index].get();
    shader->use();
    shader->setUniformInt("mapKd", 0);
    shader->setUniformVec3("material.ks", glm::vec3(0.2, 0.2, 0.2));
    shader->setUniformVec3("material.ka", glm::vec3(0.2, 0.2, 0.2));
    shader->setUniformFloat("material.ns", 314);
    _material[0]->bind(0);
    _model[_model_current_index * 2]->draw();
    _material[1]->bind(0);
    _model[_model_current_index * 2 + 1]->draw();
    frame_shoot_control();
    // debug使用
    // static int times = 0;
    //     if(times%40==0)
    //     {
    //     frame_shoot();
    //     }else if(times%240<=40){
    //         frame_shoot();
    //     }
    //     times++;
}
void Shooter::frame_shoot_control()
{
    if (_model_current_index == 0)
    {
        if (_is_shooting)
        {
            _model_current_index = 1;
            _is_shooting = false;
        }
    }
    else if (_model_current_index == 5)
    {
        if (_is_shooting)
        {
            _model_current_index = 1;
            _is_shooting = false;
        }
    }
    else if (_model_current_index == 9)
    {
        _model_current_index = 0;
    }
    else
    {
        _model_current_index++;
    }
}
void Shooter::frame_shoot()
{
    _is_shooting = true;
}
