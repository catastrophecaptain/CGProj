#ifndef CSGO_BULLET_H
#define CSGO_BULLET_H
#include "glm/glm.hpp"
#include "iostream"
#include "shooter.hpp"
#include <ctime>
class Bullet{
public:
    Bullet();
    ~Bullet() = default;
    float _speed = 10.0f;
    float _radius = 0.5;
    glm::vec3 getCurrentPosition();
    void shoot(Shooter* shooter);
private:
    glm::vec3 _direction = glm::vec3 (0.0f, 0.0f, 0.0f);
    glm::vec3 _position = glm::vec3 (0.0f,0.0f, 0.0f);
    std::time_t _shooting_time = std::time(nullptr);
};
#endif //CSGO_BULLET_H
