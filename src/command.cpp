#include "command.hpp"
Command::Command(Engine* engine): _engine(engine){
    _cameras.resize(2);
    _cameras[0].reset(new PerspectiveCamera(
            glm::radians(50.0f), 1.0f * _engine->_windowWidth/ _engine->_windowHeight, 0.1f, 1000.0f));
    _cameras[0]->transform.position.z = 10.0f;
    _cameras[1].reset(new PerspectiveCamera(
            glm::radians(50.0f), 1.0f * _engine->_windowWidth / _engine->_windowHeight, 0.1f, 1000.0f));
    _cameras[1]->transform.position.z = 10.0f;
    _cameras[1]->transform.position.y = 100.0f;
    float angle = glm::radians(90.0f);
    glm::vec3 axis = glm::vec3(-1.0f, 0.0f, 0.0f);
    _cameras[1]->transform.rotation = glm::angleAxis(angle, axis) * _cameras[1]->transform.rotation;

};

Command::~Command() = default;

void Command::handleInput() {
    bool switch_to_roam =(_engine->_input.keyboard.keyStates[GLFW_KEY_1] != GLFW_RELEASE);
    bool switch_to_fpv = (_engine->_input.keyboard.keyStates[GLFW_KEY_0] != GLFW_RELEASE);
    bool shoot = (_engine->_input.keyboard.keyStates[GLFW_KEY_F] != GLFW_RELEASE);
    bool jump = (_engine->_input.keyboard.keyStates[GLFW_KEY_1] != GLFW_RELEASE);
    if (switch_to_roam) {
        transforms.push_back(_engine->_camera->transform);
        _cameras[1]->transform.position = glm::vec3 (transforms[0].position.x, 150.0f, transforms[0].position.z);
        _engine->_camera->transform = _cameras[1]->transform;
        _view = 1;
    }

    if (switch_to_fpv) {
        _cameras[0]->transform = transforms[0];
        transforms.clear();
        _engine->_camera->transform = _cameras[0]->transform;
        _view = 0;
    }

    if (_view == 0 && shoot) {
        _engine->shooter->frame_shoot();
        Bullet* _new_bullet = new Bullet();
        _new_bullet->shoot(_engine->shooter);
        _engine->_bullets.push_back(_new_bullet);
    }
    if (jump) {
        if (_view == 1) { //第三视角不可跳跃
            return;
        }
        // 跳跃函数接口
    }
    int _num_ghosts = _engine->_ghosts.size();
    glm::vec3 _shooter_pos = _engine->shooter->_transform.position;
    for (auto ghost : _engine->_ghosts) {
        float _deltaX = ghost->_model->transform.position.x - _shooter_pos.x;
        float _deltaZ = ghost->_model->transform.position.z - _shooter_pos.z;
        if (sqrt(_deltaX * _deltaX + _deltaZ * _deltaZ) <= ghost->_range_radius) {
            ghost->_move_dir = _shooter_pos - ghost->_model->transform.position;
        } else {
            ghost->_move_dir = glm::vec3 (0.0f);
        }
    }
}

void Command::generateGhost() {
    int _num_ghosts = _engine->_ghosts.size();
    std::uniform_int_distribution<> _num_generator(1, 500);
    std::random_device rd;
    std::mt19937 gen(rd());
    for (int i = 0; i < _num_ghosts; i++) {
        int x = _num_generator(gen);
        int y = _num_generator(gen);
        int z = _num_generator(gen);
        float ghost_scale = 6.0f;
        _engine->_ghosts[i]->_model->transform.scale = glm::vec3(ghost_scale, ghost_scale, ghost_scale);
        _engine->_ghosts[i]->_model->transform.position = glm::vec3 ((x - 250.0f) * 0.8f, fmod(y, 40.0f) + 20.0f, (z - 250.0f) * 2.5f);
    }
}