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
    if (switch_to_roam) {
        pos.push_back(_engine->_camera->transform.position);
        _cameras[1]->transform.position = glm::vec3 (pos[0].x, 100.0f, pos[0].z);
        _engine->_camera->transform = _cameras[1]->transform;
        _view = 1;
    }

    if (switch_to_fpv) {
        _cameras[0]->transform.position = pos[0];
        pos.clear();
        _engine->_camera->transform = _cameras[0]->transform;
        _view = 0;
    }

    
}