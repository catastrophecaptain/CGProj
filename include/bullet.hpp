#pragma once
#include "glm/glm.hpp"
#include "iostream"
#include "object.hpp"
class Bullet : public Object
{
    public:
    Bullet(Engine *engine, glm::vec3 position, glm::vec3 direction, float speed=300.0f);
    ~Bullet();
    glm::vec3 _direction;
    glm::vec3 _position;
    glm::vec3 _position_old;

    float _speed;
    std::vector<Segment> getSegments() override;
    void collidedBy(Object *other) override;
    void plot() override;
    void renew() override;
    bool _is_to_delete = false;
};
