#include "object.hpp"
#include "engine.hpp"
void Object::normalizingShader(int index,Transform * model)
{
    auto shader = _engine->_shaders[index].get();
    shader->use();
    if(model!=nullptr)
    {
        shader->setUniformMat4("model", model->getLocalMatrix());
    }
    shader->setUniformVec3("cameraPos", _engine->_camera->transform.position);
    shader->setUniformVec3("directionalLight.direction", _engine->_light_directional->transform.getFront());
    shader->setUniformFloat("directionalLight.intensity", _engine->_light_directional->intensity);
    shader->setUniformVec3("directionalLight.color", _engine->_light_directional->color);
    shader->setUniformVec3("spotLight.position", _engine->_light_point->transform.position);
    // shader->setUniformVec3("spotLight.direction", _engine->_light_point->transform.getFront());
    shader->setUniformFloat("spotLight.intensity", _engine->_light_point->intensity);
    shader->setUniformVec3("spotLight.color",_engine-> _light_point->color);
    shader->setUniformFloat("spotLight.kc", _engine->_light_point->kc);
    shader->setUniformFloat("spotLight.kl", _engine->_light_point->kl);
    shader->setUniformFloat("spotLight.kq", _engine->_light_point->kq);
    shader->setUniformVec3("ambientLight.color", _engine->_light_ambient->color);
    shader->setUniformFloat("ambientLight.intensity", _engine->_light_ambient->intensity);
    shader->setUniformMat4("view", _engine->_camera->getViewMatrix());
    shader->setUniformMat4("projection", _engine->_camera->getProjectionMatrix());
}