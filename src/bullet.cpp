//
// Created by apple on 2024/5/30.
//
#include "bullet.hpp"
Bullet::Bullet() = default;

void Bullet::shoot(Shooter* shooter) {
    _shooting_time = std::time(nullptr);
    _position = shooter->_transform.position;
    _direction = shooter->_transform.getFront();
}

// 返回当前子弹此时应该在的位置，用于判断和ghost的碰撞
glm::vec3 Bullet::getCurrentPosition() {
    std::time_t _current_time = std::time(nullptr);
    time_t time_window = (int )(_current_time - _shooting_time);
    return {_position.x+time_window*_direction.x, _position.y+time_window*_direction.y, _position.z+time_window*_direction.z};
}