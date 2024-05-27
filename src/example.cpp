#include <example.hpp>
#include <engine.hpp>
Example::Example(Engine *engine) : Object(engine, Category::ENEMY)
{
    engine->addObject(this);
    init();
};
void Example::init()
{
    std::string path = "media";
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
};