#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <object.hpp>
class Example : public Object
{
    public:
    Example(Engine *engine);
    void init();
};