#include <map.hpp>
#include <engine.hpp>
#include <tiny_obj_loader.h>
#include <random>
#include <algorithm>
Map::Map(Engine *engine, std::string _material_path, std::string _model_path) : Object(engine, Category::MAP)
{
    engine->addObject(this);
    init(_engine->_assetRootDir + _material_path, _engine->_assetRootDir + _model_path);
}
void Map::init(std::string _material_path, std::string _model_path)
{
    for (int i = 0; i < _plane_material_cnt; i++)
    {
        std::string material_path = _material_path + std::to_string(i) + ".png";
        _plane_materials.push_back(std::unique_ptr<ImageTexture2D>(new ImageTexture2D(material_path)));
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    std::string::size_type index = _model_path.find_last_of("/");
    std::string mtlBaseDir = _model_path.substr(0, index + 1);

    if (!tinyobj::LoadObj(
            &attrib, &shapes, &materials, &warn, &err, _model_path.c_str(), mtlBaseDir.c_str()))
    {
        throw std::runtime_error("load " + _model_path + " failure: " + err);
    }

    if (!warn.empty())
    {
        std::cerr << "Loading model " + _model_path + " warnings: " << std::endl;
        std::cerr << warn << std::endl;
    }

    if (!err.empty())
    {
        throw std::runtime_error("Loading model " + _model_path + " error:\n" + err);
    }

    for (const auto &shape : shapes)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::unordered_map<Vertex, uint32_t> uniqueVertices;
        std::cout << shape.name << std::endl;
        bool is_plane = !(shape.name.compare("Plane"));
        for (const auto &index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.position.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.position.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.position.z = attrib.vertices[3 * index.vertex_index + 2];

            if (index.normal_index >= 0)
            {
                vertex.normal.x = attrib.normals[3 * index.normal_index + 0];
                vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
                vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
            }

            if (index.texcoord_index >= 0)
            {
                vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.texCoord.y = attrib.texcoords[2 * index.texcoord_index + 1];
            }

            // check if the vertex appeared before to reduce redundant data
            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }

        if (is_plane)
        {
            // for (int i = 0; i < vertices.size(); i++)
            // {
            //     vertices[i].texCoord.x = std::rand() % 10 / 100.0f;
            //     vertices[i].texCoord.y *= std::rand() % 10 / 100.0f;
            // }
            Model *temp = new Model(vertices, indices);
            std::cout << "\n\nplane\n\n"
                      << std::endl;
            _plane_model = std::unique_ptr<Model>(temp);
        }
        else
        {
            Model *temp = new Model(vertices, indices);
            _models.push_back(std::unique_ptr<Model>(temp));
            MapMaterial *temp2 = new MapMaterial();
            temp2->ka.r = std::max(0.0, materials[shape.mesh.material_ids[0]].ambient[0] + (std::rand() % 400 - 200) / 1000.0);
            temp2->ka.g = std::max(0.0, materials[shape.mesh.material_ids[0]].ambient[1] + (std::rand() % 400 - 200) / 1000.0);
            temp2->ka.b = std::max(0.0, materials[shape.mesh.material_ids[0]].ambient[2] + (std::rand() % 400 - 200) / 1000.0);
            temp2->kd.r = std::max(0.0, materials[shape.mesh.material_ids[0]].diffuse[0] + (std::rand() % 400 - 200) / 1000.0);
            temp2->kd.g = std::max(0.0, materials[shape.mesh.material_ids[0]].diffuse[1] + (std::rand() % 400 - 200) / 1000.0);
            temp2->kd.b = std::max(0.0, materials[shape.mesh.material_ids[0]].diffuse[2] + (std::rand() % 400 - 200) / 1000.0);
            temp2->ks.r = std::max(0.0, materials[shape.mesh.material_ids[0]].specular[0] + (std::rand() % 400 - 200) / 1000.0);
            temp2->ks.g = std::max(0.0, materials[shape.mesh.material_ids[0]].specular[1] + (std::rand() % 400 - 200) / 1000.0);
            temp2->ks.b = std::max(0.0, materials[shape.mesh.material_ids[0]].specular[2] + (std::rand() % 400 - 200) / 1000.0);
            temp2->ns = materials[shape.mesh.material_ids[0]].shininess;
            _materials.push_back(std::unique_ptr<MapMaterial>(temp2));
        }
    }
}
void Map::plot()
{
    normalizingShader(_shader_index, &(_transform));
    auto shader = _engine->_shaders[_shader_index].get();
    shader->use();

    // shader->setUniformVec3("material.ks", glm::vec3(0.5, 0.5, 0.5));
    // shader->setUniformVec3("material.ka", glm::vec3(1.0, 1.0, 1.0));
    // shader->setUniformVec3("material.kd", glm::vec3(0.5, 0.5, 0.5));
    // shader->setUniformFloat("material.ns", 100.0f);

    for (int i = 0; i < _models.size(); i++)
    {
        shader->setUniformVec3("material.ks", _materials[i]->kd);
        shader->setUniformVec3("material.ka", _materials[i]->kd);
        shader->setUniformVec3("material.kd", _materials[i]->kd);
        shader->setUniformFloat("material.ns", _materials[i]->ns);
        _models[i]->draw();
    }
    normalizingShader(0, &(_transform));
    auto shader2 = _engine->_shaders[0].get();
    shader2->use();
    shader2->setUniformInt("mapKd", 0);
    shader2->setUniformVec3("material.ks", glm::vec3(0.2, 0.2, 0.2));
    shader2->setUniformVec3("material.ka", glm::vec3(0.2, 0.2, 0.2));
    shader2->setUniformFloat("material.ns", 314);
    shader2->setUniformVec3("scale", glm::vec3(2.0f, 2.0f, 2.0f));
    _plane_materials[_plane_material_index]->bind(0);
    _plane_model->draw();
}
int Map::_shader_index = 1;
void Map::setShaderIndex(int index)
{
    _shader_index = index;
}
std::vector<Box> Map::getBoxs()
{
    std::vector<Box> boxs;
    for (int i = 0; i < _models.size(); i++)
    {
        BoundingBox box = _models[i]->getBoundingBox();
        boxs.push_back(Box(box.min, box.max, _transform));
    }
    BoundingBox box = _plane_model->getBoundingBox();
    boxs.push_back(Box(box.min, box.max, _transform));
    return boxs;
}
void Map::renew()
{
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_1] != GLFW_RELEASE)
    {
        _plane_material_index = 0;
    }
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_2] != GLFW_RELEASE)
    {
        std::cout << "renew" << std::endl;
        _plane_material_index = 1;
    }
}