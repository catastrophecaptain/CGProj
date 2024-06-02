#include <object.hpp>
#include <base/model.h>
#include <memory>
#include <base/glsl_program.h>
#include <base/texture.h>
#include <base/texture2d.h>
#include <vector>
struct MapMaterial
{
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    float ns;
};

class Map : public Object
{
    public:
    Map(Engine *engine,std::string _material_path="texture/map/",std::string _model_path="obj/map/map_final.obj");
    void init(std::string _material_path,std::string _model_path);
    void plot() override;
    void static setShaderIndex(int index);
    std::vector<Box> getBoxs() override;
    void renew() override;
    static int _shader_index;
    std::vector<std::unique_ptr<MapMaterial>> _materials;
    std::vector<std::unique_ptr<ImageTexture2D>> _plane_materials;
    std::unique_ptr<Model> _plane_model;
    int _plane_material_index=0;
    int _plane_material_cnt=2;
    std::vector<std::unique_ptr<Model>> _models;
    Transform _transform;
    // bool _is_loaded = false;
};