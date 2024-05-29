#pragma once
#include <vector>
#include <base/transform.h>
class Engine;
class Segment
{
};
class Box
{
};
enum class Category
{
    GHOST,
    SHOOTER,
    BULLET,
    BOX,
    AMBIENCE,
    MAP
};
class Object
{
public:
    Object(Engine *engine, Category category, bool fixed = false) : _engine(engine), _fixed(fixed), _category(category){};
    virtual ~Object(){};
    virtual void renew() {};
    virtual void plot() = 0;
    virtual bool isFixed()
    {
        return _fixed;
    }
    virtual std::vector<Segment> getSegments()
    {
        return std::vector<Segment>();
    }
    virtual std::vector<Box> getBoxs()
    {
        return std::vector<Box>();
    }
    virtual void collidedBy(Object *other)
    {
        return;
    }
    virtual Category getCategory()
    {
        return _category;
    }
    void normalizingShader(int index,Transform *transform=nullptr);
    
    Engine *_engine;
    bool _fixed;
    Category _category;
};