#include <shooter.hpp>
#include <engine.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "bullet.hpp"
#include <fstream>
Shooter::Shooter(Engine *engine, glm::vec3 scale, glm::vec3 position, glm::quat rotation,
                 std::vector<std::string> _material_path, std::vector<std::string> _model_path) : Object(engine, Category::SHOOTER)
{
    engine->addObject(this, true);
    init(_material_path, _model_path);
    _model_transform.scale = scale;
    _model_transform.position = position;
    _model_transform.rotation = rotation;
    change_stage(_engine->_stage);
    original_position = position + glm::vec3(0, 10, 0);
    _bounding_box = BoundingBox{glm::vec3(-8.0f, -45.0f, -8.0f), glm::vec3(8.0f, 5.0f, 8.0f)};
    for (int i = 0; i < _old_cnt; i++)
    {
        _transform_old.push_back(_model_transform);
    }
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
    if (!_is_view_mode)
    {
        normalizingShader(_shader_index, &_model_transform);
        auto shader = _engine->_shaders[_shader_index].get();
        // todo
        glm::mat4 static_matrix;
        static_matrix[0] = glm::vec4(1, 0, 0, 0);
        static_matrix[1] = glm::vec4(0, 1, 0, 0);
        static_matrix[2] = glm::vec4(0, 0, 1, 0);
        static_matrix[3] = glm::vec4(0, 0, -10, 1);
        shader->use();
        shader->setUniformMat4("view", static_matrix);
        shader->setUniformInt("mapKd", 0);
        shader->setUniformVec3("material.ks", glm::vec3(0.2, 0.2, 0.2));
        shader->setUniformVec3("material.ka", glm::vec3(0.2, 0.2, 0.2));
        shader->setUniformFloat("material.ns", 314);
        shader->setUniformVec3("scale", glm::vec3(2.0f, 2.0f, 2.0f));
        _material[0]->bind(0);
        _model[_model_current_index * 2]->draw();
        _material[1]->bind(0);
        _model[_model_current_index * 2 + 1]->draw();
        frame_shoot_control();
    }
}
void Shooter::frame_shoot_control()
{
    if (_model_current_index == 0)
    {
        if (_is_shooting)
        {
            _model_current_index = 1;
            mciSendString("open ../media/sound/shoot.wav alias shoot", NULL, 0, NULL);
            mciSendString("play shoot", NULL, 0, NULL);
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
        else
        {
            _model_current_index++;
        }
    }
    else if (_model_current_index == 9)
    {
        _model_current_index = 0;
        mciSendString("stop shoot", NULL, 0, NULL); // 停止音乐播放
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
void Shooter::renew()
{
    //     if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE)
    // {
    //     std::cout << "W" << std::endl;
    //     _camera->transform.position += _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
    // }

    // if (_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE)
    // {
    //     std::cout << "A" << std::endl;
    //     _camera->transform.position -= _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
    // }

    // if (_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE)
    // {
    //     std::cout << "S" << std::endl;
    //     _camera->transform.position -= _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
    // }

    // if (_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE)
    // {
    //     std::cout << "D" << std::endl;
    //     _camera->transform.position += _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
    // }

    // if (_input.mouse.move.xNow != _input.mouse.move.xOld)
    // {
    //     std::cout << "mouse move in x direction" << std::endl;
    //     _camera->transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.xNow + _input.mouse.move.xOld)), glm::vec3(0.0f, 1.0f, 0.0f)) * _camera->transform.rotation;
    // }

    // if (_input.mouse.move.yNow != _input.mouse.move.yOld)
    // {
    //     std::cout << "mouse move in y direction" << std::endl;
    //     _camera->transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.yNow + _input.mouse.move.yOld)), _camera->transform.getRight()) * _camera->transform.rotation;
    // }
    // std::cout << _transform.position.x << " " << _transform.position.y << " " << _transform.position.z << std::endl;
    _transform_old.push_back(_transform);
    _transform_old.erase(_transform_old.begin());
    bool _is_landed = false;
    _transform.position.y -= 10.0;
    for (auto it : _engine->_objects)
    {
        for (auto box : it->getBoxs())
        {
            for (auto segmant : getSegments())
            {
                if (it->getCategory() == Category::MAP && _engine->checkCollision(box, segmant))
                {
                    _is_landed = true;
                }
            }
        }
    }
    _transform = _transform_old.back();
    if (_is_landed)
    {
        _up_speed = 0;
    }
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_O]!= GLFW_RELEASE)
    {
        outObj();
    }
    if (!_is_end)
    {
        if (_engine->_input.mouse.move.xNow != _engine->_input.mouse.move.xOld)
        {
            _transform.rotation = glm::angleAxis(glm::radians(_angle_speed * (-_engine->_input.mouse.move.xNow + _engine->_input.mouse.move.xOld)), glm::vec3(0.0f, 1.0f, 0.0f)) * _transform.rotation;
        }
        if (_engine->_input.mouse.move.yNow != _engine->_input.mouse.move.yOld)
        {
            _transform.rotation = glm::angleAxis(glm::radians(_angle_speed * (-_engine->_input.mouse.move.yNow + _engine->_input.mouse.move.yOld)), _transform.getRight()) * _transform.rotation;
        }
        if (_is_view_mode)
        {
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE)
            {
                _transform.position += _transform.getFront() * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE)
            {
                _transform.position -= _transform.getFront() * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE)
            {
                _transform.position -= _transform.getRight() * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE)
            {
                _transform.position += _transform.getRight() * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.mouse.scroll.xOffset != 0)
            {
                _engine->_camera->fovy *= std::pow(1.01, -_engine->_input.mouse.scroll.xOffset);
            }
            if (_engine->_input.mouse.scroll.yOffset != 0)
            {
                _engine->_camera->fovy *= std::pow(1.01, -_engine->_input.mouse.scroll.yOffset);
            }
        }
        else
        {
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE)
            {
                _transform.position += normalize(glm::vec3(_transform.getFront().x, 0, _transform.getFront().z)) * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE)
            {
                _transform.position -= normalize(glm::vec3(_transform.getFront().x, 0, _transform.getFront().z)) * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE)
            {
                _transform.position -= normalize(glm::vec3(_transform.getRight().x, 0, _transform.getRight().z)) * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE)
            {
                _transform.position += normalize(glm::vec3(_transform.getRight().x, 0, _transform.getRight().z)) * _speed * _engine->_deltaTime;
            }
            if (_engine->_input.mouse.press.left)
            {
                std::cout << "mouse left" << std::endl;
                frame_shoot();
                Bullet *bullet = new Bullet(_engine, _transform_old.back().position, _transform_old.back().getFront(), 10000.0);
            }
            if (_engine->_input.keyboard.keyStates[GLFW_KEY_SPACE] != GLFW_RELEASE && _is_landed)
            {
                _up_speed = _up_speed_once;
            }
            if (!_is_landed)
            {

                _up_speed -= _gravity * _engine->_deltaTime;
            }
            _transform.position.y += _up_speed * _engine->_deltaTime;
        }
        if (_engine->_input.keyboard.keyStates[GLFW_KEY_R] != GLFW_RELEASE)
        {
            _transform.position = original_position;
        }
        renew_camera();
    }
    static bool is_play = false;
    if (!_is_end && !_is_view_mode && _is_landed && glm::length(_transform.position - _transform_old.back().position) > 0.1)
    {
        if (!is_play)
        {
            mciSendString("open ../media/sound/walk.wav alias walk", NULL, 0, NULL);
            mciSendString("play walk", NULL, 0, NULL);
            is_play = true;
        }
    }
    else
    {
        mciSendString("stop walk", NULL, 0, NULL);
        is_play = false;
    }
}
void Shooter::change_stage(EngineStage stage)
{
    switch (stage)
    {
    case EngineStage::START:
        _is_end = false;
        _is_view_mode = true;
        break;
    case EngineStage::RUN:
        _engine->_camera->fovy = _fovy_init;
        _is_end = false;
        _is_view_mode = false;
        break;
    case EngineStage::END:
        _is_end = true;
        _up_speed = 0;
        break;
    }
}
void Shooter::renew_camera()
{
    _engine->_camera->transform.position = _transform.position;
    _engine->_camera->transform.rotation = _transform.rotation;
}
std::vector<Box> Shooter::getBoxs()
{
    std::vector<Box> boxs;
    Transform transform = _transform_old.back();
    transform.rotation = glm::quat{1.0, 0.0, 0.0, 0.0};
    boxs.push_back(Box{_bounding_box.min, _bounding_box.max, transform});
    return boxs;
}
std::vector<Segment> Shooter::getSegments()
{
    std::vector<Segment> segments;
    if (!_is_view_mode)
    {
        glm::vec3 vertex_1[20];
        Transform _transform_1{_transform};
        Transform _transform_old_1{_transform_old.back()};
        _transform_1.rotation = glm::quat{1.0, 0.0, 0.0, 0.0};
        _transform_old_1.rotation = glm::quat{1.0, 0.0, 0.0, 0.0};
        vertex_1[0] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.min, 1.0f);
        vertex_1[1] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.max, 1.0f);
        vertex_1[2] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.min.x, _bounding_box.min.y, _bounding_box.max.z, 1.0f);
        vertex_1[3] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.min.x, _bounding_box.max.y, _bounding_box.min.z, 1.0f);
        vertex_1[4] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.min.x, _bounding_box.max.y, _bounding_box.max.z, 1.0f);
        vertex_1[5] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.max.x, _bounding_box.min.y, _bounding_box.min.z, 1.0f);
        vertex_1[6] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.max.x, _bounding_box.min.y, _bounding_box.max.z, 1.0f);
        vertex_1[7] = _transform_1.getLocalMatrix() * glm::vec4(_bounding_box.max.x, _bounding_box.max.y, _bounding_box.min.z, 1.0f);
        vertex_1[8] = (vertex_1[0] + vertex_1[2]) / 2.0f;
        vertex_1[9] = (vertex_1[0] + vertex_1[3]) / 2.0f;
        vertex_1[10] = (vertex_1[0] + vertex_1[5]) / 2.0f;
        vertex_1[11] = (vertex_1[1] + vertex_1[4]) / 2.0f;
        vertex_1[12] = (vertex_1[1] + vertex_1[6]) / 2.0f;
        vertex_1[13] = (vertex_1[1] + vertex_1[7]) / 2.0f;
        vertex_1[14] = (vertex_1[1] + vertex_1[8]) / 2.0f;
        glm::vec3 vertex_2[20];
        vertex_2[0] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.min, 1.0f);
        vertex_2[1] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.max, 1.0f);
        vertex_2[2] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.min.x, _bounding_box.min.y, _bounding_box.max.z, 1.0f);
        vertex_2[3] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.min.x, _bounding_box.max.y, _bounding_box.min.z, 1.0f);
        vertex_2[4] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.min.x, _bounding_box.max.y, _bounding_box.max.z, 1.0f);
        vertex_2[5] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.max.x, _bounding_box.min.y, _bounding_box.min.z, 1.0f);
        vertex_2[6] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.max.x, _bounding_box.min.y, _bounding_box.max.z, 1.0f);
        vertex_2[7] = _transform_old_1.getLocalMatrix() * glm::vec4(_bounding_box.max.x, _bounding_box.max.y, _bounding_box.min.z, 1.0f);
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
    }
    return segments;
}
void Shooter::collidedBy(Object *other)
{
    switch (other->getCategory())
    {
    case Category::GHOST:
    {
        std::cout << "collided by ghost" << std::endl;
        if (!_is_end)
        {
            std::cout << "You are dead" << std::endl;
            _engine->_stage = EngineStage::END;
        }
        break;
    }
    case Category::BULLET:
    {
        break;
    }
    case Category::MAP:
    {
        std::cout << "collided by map" << std::endl;
        glm::vec3 position = _transform.position;
        bool solve = false;
        for (int i = 0; i < 3; i++)
        {

            _transform.position[i] = _transform_old.back().position[i];
            solve = true;
            for (auto box : other->getBoxs())
            {
                for (auto segmant : getSegments())
                {
                    if (_engine->checkCollision(box, segmant))
                    {
                        solve = false;
                    }
                }
            }
            if (!solve)
            {
                _transform.position = position;
            }
            else
            {
                if(i==1)
                {
                    _up_speed = 0;
                }
                break;
            }
        }
        if (solve)
        {
            break;
        }
        for (int i = 0; i < 3; i++)
        {
            _transform.position = _transform_old.back().position;
            _transform.position[i] = position[i];
            solve = true;
            for (auto box : other->getBoxs())
            {
                for (auto segmant : getSegments())
                {
                    if (_engine->checkCollision(box, segmant))
                    {
                        solve = false;
                    }
                }
            }
            if(solve)
            {
                if(i!=1)
                {
                    _up_speed = 0;
                }
                break;
            }
        }
        if (solve)
        {
            break;
        }
        _up_speed=0;
        _transform.position = _transform_old[_transform_old.size()-1].position;
        std::cout << _engine->_t_min << std::endl;
    }
    case Category::SHOOTER:
    {
        break;
    }
    }
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_R] != GLFW_RELEASE)
    {
        _transform.position = original_position;
    }
    renew_camera();
}
void Shooter::outObj()
{
    static int cnt = 0;
    glm::mat4 static_matrix;
    static_matrix[0] = glm::vec4(1, 0, 0, 0);
    static_matrix[1] = glm::vec4(0, 1, 0, 0);
    static_matrix[2] = glm::vec4(0, 0, 1, 0);
    static_matrix[3] = glm::vec4(0, 0, -10, 1);
    std::string filename = _engine->_assetRootDir + "out/gun" + std::to_string(cnt) + ".obj";
    cnt++;
    std::ofstream file(filename);
    const std::vector<Vertex> &vertices = (_model[_model_current_index*2+1])->getVertices();
    const std::vector<uint32_t> indices = (_model[_model_current_index*2+1])->getIndices();
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    // "    fPosition = vec3(model * vec4(aPosition, 1.0f));\n"
    // "    fNormal = mat3(transpose(inverse(model))) * aNormal;\n"
    glm::mat4 model_apply = glm::inverse(_engine->_camera->getViewMatrix()) * static_matrix * _model_transform.getLocalMatrix();
    glm::mat3 normal_apply = glm::mat3(transpose(inverse(glm::inverse(_engine->_camera->getViewMatrix()) * static_matrix * _model_transform.getLocalMatrix())));
    file<<"o Gun"<<std::endl;
    // 写入顶点位置
    for (auto vertex : vertices)
    {
        vertex.position = glm::vec3((model_apply * glm::vec4(vertex.position, 1.0f)));
        file << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << std::endl;
    }

    // 写入顶点纹理坐标
    for (const auto &vertex : vertices)
    {
        file << "vt " << vertex.texCoord.x << " " << vertex.texCoord.y << std::endl;
    }

    // 写入顶点法线
    for (auto vertex : vertices)
    {
        vertex.normal = normal_apply * vertex.normal;
        file << "vn " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << std::endl;
    }

    // 写入面信息
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        // OBJ文件索引从1开始
        uint32_t idx1 = indices[i] + 1;
        uint32_t idx2 = indices[i + 1] + 1;
        uint32_t idx3 = indices[i + 2] + 1;
        // 假设每个顶点的位置、纹理坐标和法线索引是相同的
        file << "f " << idx1 << "/" << idx1 << "/" << idx1 << " "
             << idx2 << "/" << idx2 << "/" << idx2 << " "
             << idx3 << "/" << idx3 << "/" << idx3 << std::endl;
    }

    file.close();
    std::cout << "Model exported to " << filename << std::endl;
}
