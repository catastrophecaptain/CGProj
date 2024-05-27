#pragma once
#include <vector>
class Engine;
class Segment
{
};
class Box
{
};
enum class Category
{
    ENEMY,
    SHOOTER,
    BULLET,
    WALL,
    BOX
};
class Object
{
public:
    virtual ~Object(){};
    virtual void renew() = 0;
    virtual void plot() = 0;
    virtual bool isfixed()
    {
        return _fixed;
    };
    virtual std::vector<Segment> getsegments() = 0;
    virtual std::vector<Box> getboxs() = 0;
    virtual void collidedby(Object *other) = 0;
    virtual Category getcategory() = 0;
    Engine *_engine;
    bool _fixed;
};