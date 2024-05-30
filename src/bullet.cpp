//
// Created by apple on 2024/5/30.
//
#include "bullet.hpp"
#include "engine.hpp"
Bullet::Bullet(Engine *engine, glm::vec3 position, glm::vec3 direction, float speed) : Object(engine, Category::BULLET), _position(position), _direction(direction), _speed(speed)
{
    _position_old = _position;
    _engine->addObject(this, true);
}
Bullet::~Bullet()
{
}
void Bullet::plot()
{
    // auto shader=_engine->_shaders[2].get();
    // glm::mat4 view = _engine->_camera->getViewMatrix();
    // glm::mat4 projection = _engine->_camera->getProjectionMatrix();
    // glm::vec3 position_old=projection*view*glm::vec4(_position_old,1.0f);
    // glm::vec3 position=projection*view*glm::vec4(_position,1.0f);

    // shader->use();
    //  float vertices[] = {
    //     position_old.x, position_old.y, position_old.z,
    //     position.x, position.y, position.z
    // };
    // unsigned int VBO, VAO;
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);
    // // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // glBindVertexArray(VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0); 
    // glBindVertexArray(0);
    // glDrawArrays(GL_LINES, 0, 2);
    // return;
    return;
}
void Bullet::renew()
{
    _position_old = _position;
    _position += _direction * _speed * _engine->_deltaTime;
    if (_position.x > 10000 || _position.x < -10000 || _position.y > 10000 || _position.y < -10000 || _position.z > 10000 || _position.z < -10000)
    {
        if (!_is_to_delete)
        {
            _engine->_objects_to_delete.push_back(this);
            _is_to_delete = true;
        }
    }
}
std::vector<Segment> Bullet::getSegments()
{
    return std::vector<Segment>{Segment{_position_old, _position}};
}
void Bullet::collidedBy(Object *other)
{
        if (!_is_to_delete&&other->getCategory() != Category::SHOOTER)
        {
            _engine->_objects_to_delete.push_back(this);
            _is_to_delete = true;
        }
}