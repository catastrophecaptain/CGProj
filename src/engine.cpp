#include <engine.hpp>
#include <object.hpp>
#include <command.hpp>
#include <iostream>
#include "ambience.hpp"
#include <base/light.h>
#include <glm/gtc/matrix_transform.hpp>
//#include "shooter.hpp"
//#include "ghost.hpp"
#include "map.hpp"
Engine::Engine(const Options &options) : Application(options)
{
    _command = new Command(this);
    start();
};
Engine::~Engine()
{
    for (auto object : _objects)
    {
        delete object;
    }
    delete _command;
};
void Engine::start()
{
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
    _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
    glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
    _camera.reset(new PerspectiveCamera(
        glm::radians(50.0f), 1.0f * _windowWidth / _windowHeight, 0.1f, 1000.0f));
    _camera->transform.position.z = 10.0f;
    _stage = EngineStage::START;
    initShaders();
    initlights();
    Ambience *ambience = new Ambience(this);
    shooter = new Shooter(this);
    glm::mat4 scale = glm::mat4 (1.0f);
    float shooter_scale = 8.0f;
    shooter->_transform.scale = glm::vec3 (shooter_scale, shooter_scale, shooter_scale);
    shooter->_transform.position = glm::vec3 (0.0f, -1.0f, 7.0f);
    float angle = glm::radians(180.0f);
    shooter->_transform.rotation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
    addObject(shooter, true);
    int num_ghosts = 10;
    for (int i = 0; i < num_ghosts; i ++) {
        Ghost* _new_ghost = new Ghost(this);
        _ghosts.push_back(_new_ghost);
        addObject(_new_ghost, true);
    }
    _command->generateGhost();
     Map *map = new Map(this);
    // Example *example = new Example(this);
};
void Engine::getCommand()
{
    _input.forwardState();
};
void Engine::renew()
{
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    _command->handleInput();
    for (auto ghost : _ghosts) {
        ghost->_model->transform.position += _deltaTime * ghost->_speed * ghost->_move_dir;
    }
    cameraRenew();
    for (auto object : _objects)
    {
        object->renew();
    }
    for (auto object : _objects_move)
    {
        object->renew();
    }
};
void Engine::check()
{
    for (auto object : _objects)
    {
        if (object->isFixed())
        {
            for (auto object_move : _objects_move)
            {
                if (checkCollision(object, object_move))
                {
                    object->collidedBy(object_move);
                    object_move->collidedBy(object);
                }
            }
        }
    }
};
void Engine::plot()
{
    for (auto object : _objects)
    {
        object->plot();
    }
};
bool Engine::checkCollision(Object *fixed, Object *move)
{
    return false;
};
void Engine::handleInput()
{
    renew();
    check();
    getCommand();
};
void Engine::renderFrame()
{
    showFpsInWindowTitle();
    plot();
};
void Engine::addObject(Object *object, bool move)
{
    if (move)
    {
        _objects_move.push_back(object);
        _objects.push_back(object);
    }
    else
    {
        _objects.push_back(object);
    }
};
void Engine::deleteObject(Object *object)
{
    for (auto it = _objects.begin(); it != _objects.end(); it++)
    {
        if (*it == object)
        {
            _objects.erase(it);
            break;
        }
    }
    for (auto it = _objects_move.begin(); it != _objects_move.end(); it++)
    {
        if (*it == object)
        {
            _objects_move.erase(it);
            break;
        }
    }
};
int Engine::addShader(const std::string &vertexShader, const std::string &fragmentShader)
{
    std::unique_ptr<GLSLProgram> shader(new GLSLProgram());
    shader->attachVertexShader(vertexShader);
    shader->attachFragmentShader(fragmentShader);
    shader->link();
    _shaders.push_back(std::move(shader));
    return _shaders.size() - 1;
};
void Engine::initShaders()
{
    std::string vsCode =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPosition;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "layout(location = 2) in vec2 aTexCoord;\n"

        "out vec3 fPosition;\n"
        "out vec3 fNormal;\n"
        "out vec2 fTexCoord;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "void main() {\n"
        "    fTexCoord = aTexCoord;\n"
        "    fPosition = vec3(model * vec4(aPosition, 1.0f));\n"
        "    fNormal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
        "}\n";
    // specularLight
    std::string fsCode_head = R"(
        #version 330 core
        in vec3 fPosition;
        in vec3 fNormal;
        in vec2 fTexCoord;
        out vec4 color;

        struct Material {
            vec3 ka;
            vec3 kd;
            vec3 ks;
            float ns;
        };
        struct DirectionalLight {
            vec3 direction;
            float intensity;
            vec3 color;
        };
        struct SpotLight {
            vec3 position;
            vec3 direction;
            float intensity;
            vec3 color;
            float kc;
            float kl;
            float kq;
        };
        struct AmbientLight {
            vec3 color;
            float intensity;
        };
        uniform vec3 cameraPos;
        uniform Material material;
        uniform DirectionalLight directionalLight;
        uniform SpotLight spotLight;
        uniform AmbientLight ambientLight;
        uniform sampler2D mapKa;
        uniform sampler2D mapKd;
        uniform sampler2D mapKs;
        uniform sampler2D mapNs;
        Material material_temp = material;
        vec3 calcDirectionalLight(vec3 normal,vec3 viewDir) 
        {
            vec3 lightDir = normalize(-directionalLight.direction);
            vec3 reflectDir = normalize(lightDir+viewDir);
            vec3 diffuse = directionalLight.color * max(dot(lightDir, normal), 0.0f) * material_temp.kd;
            vec3 specular = directionalLight.color * pow(max(dot(reflectDir, normal), 0.0f), material_temp.ns) * material_temp.ks;
            return directionalLight.intensity * (diffuse + specular);
        }
        vec3 calcSpotLight(vec3 normal,vec3 viewDir)
        {
            vec3 lightDir = normalize(spotLight.position - fPosition);
            // float theta = acos(-dot(lightDir, normalize(spotLight.direction)));
            vec3 reflectDir = normalize(lightDir+viewDir);
            vec3 diffuse = spotLight.color * max(dot(lightDir, normal), 0.0f) * material_temp.kd;
            vec3 specular = spotLight.color * pow(max(dot(reflectDir, normal), 0.0f), material_temp.ns) * material_temp.ks;
            float distance = length(spotLight.position - fPosition);
            float attenuation = 1.0f / (spotLight.kc + spotLight.kl * distance + spotLight.kq * distance * distance);
            return spotLight.intensity * attenuation * (diffuse + specular);
        }
        void main()
        {)";
    std::string fsCode_tail =
        R"(
            vec3 viewDir = normalize(cameraPos - fPosition);
            vec3 normal = normalize(fNormal);
            vec3 ambient = material_temp.ka * ambientLight.color * ambientLight.intensity;
            vec3 diffuse_specular = calcDirectionalLight(normal,viewDir)+calcSpotLight(normal,viewDir);
            color = vec4(ambient + diffuse_specular, 1.0f);

            
        }
    )";
    std::vector<std::string> fsCode_bodies;
    fsCode_bodies.push_back("material_temp.kd = texture(mapKd, fTexCoord).rgb;");
    // fsCode_bodies.push_back("");
    // fsCode_body.push_back("material.ka = texture(mapKa, fTexCoord).rgb;");
    // fsCode_body.push_back("material.ks = texture(mapKs, fTexCoord).rgb;");
    for (auto fsCode_body : fsCode_bodies)
    {
        std::string fsCode = fsCode_head + fsCode_body + fsCode_tail;
        addShader(vsCode, fsCode);
    }
    const char *map_vsCode =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPosition;\n"
        "layout(location = 1) in vec3 aNormal;\n"
        "layout(location = 2) in vec2 aTexCoord;\n"

        "out vec3 fPosition;\n"
        "out vec3 fNormal;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "void main() {\n"
        "    fPosition = vec3(model * vec4(aPosition, 1.0f));\n"
        "    fNormal = mat3(transpose(inverse(model))) * aNormal;\n"
        "    gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
        "}\n";

    const char *map_fsCode = R"(
        #version 330 core
        in vec3 fPosition;
        in vec3 fNormal;
        out vec4 color;

        struct Material {
            vec3 ka;
            vec3 kd;
            vec3 ks;
            float ns;
        };
        struct DirectionalLight {
            vec3 direction;
            float intensity;
            vec3 color;
        };
        struct SpotLight {
            vec3 position;
            vec3 direction;
            float intensity;
            vec3 color;
            // float angle;
            float kc;
            float kl;
            float kq;
        };
        struct AmbientLight {
            vec3 color;
            float intensity;
        };
        uniform vec3 cameraPos;
        uniform Material material;
        uniform DirectionalLight directionalLight;
        uniform SpotLight spotLight;
        uniform AmbientLight ambientLight;

        vec3 calcDirectionalLight(vec3 normal,vec3 viewDir) 
        {
            vec3 lightDir = normalize(-directionalLight.direction);
            vec3 reflectDir = normalize(lightDir+viewDir);
            vec3 diffuse = directionalLight.color * max(dot(lightDir, normal), 0.0f) * material.kd;
            vec3 specular = directionalLight.color * pow(max(dot(reflectDir, normal), 0.0f), material.ns) * material.ks;
            return directionalLight.intensity * (diffuse + specular);
        }
        vec3 calcSpotLight(vec3 normal,vec3 viewDir)
        {
            vec3 lightDir = normalize(spotLight.position - fPosition);
            // float theta = acos(-dot(lightDir, normalize(spotLight.direction)));
            // if (theta > spotLight.angle) {
            //     return vec3(0.0f, 0.0f, 0.0f);
            // }
            vec3 reflectDir = normalize(lightDir+viewDir);
            vec3 diffuse = spotLight.color * max(dot(lightDir, normal), 0.0f) * material.kd;
            vec3 specular = spotLight.color * pow(max(dot(reflectDir, normal), 0.0f), material.ns) * material.ks;
            float distance = length(spotLight.position - fPosition);
            float attenuation = 1.0f / (spotLight.kc + spotLight.kl * distance + spotLight.kq * distance * distance);
            return spotLight.intensity * attenuation * (diffuse + specular);
        }
        void main()
        {
            vec3 viewDir = normalize(cameraPos - fPosition);
            vec3 normal = normalize(fNormal);
            vec3 ambient = material.ka * ambientLight.color * ambientLight.intensity;
            vec3 diffuse_specular = calcDirectionalLight(normal,viewDir)+calcSpotLight(normal,viewDir);
            color = vec4(ambient+diffuse_specular, 1.0f);
        }
    )";
    int map_index = addShader(map_vsCode, map_fsCode);
    Map::setShaderIndex(map_index);
    //     const char *vsCode =
    //     "#version 330 core\n"
    //     "layout(location = 0) in vec3 aPosition;\n"
    //     "layout(location = 1) in vec3 aNormal;\n"
    //     "layout(location = 2) in vec2 aTexCoord;\n"
    //     "out vec2 fTexCoord;\n"
    //     "uniform mat4 projection;\n"
    //     "uniform mat4 view;\n"
    //     "uniform mat4 model;\n"

    //     "void main() {\n"
    //     "    fTexCoord = aTexCoord;\n"
    //     "    gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
    //     "}\n";

    // const char *fsCode =
    //     "#version 330 core\n"
    //     "in vec2 fTexCoord;\n"
    //     "out vec4 color;\n"
    //     "uniform sampler2D mapKd;\n"
    //     "void main() {\n"
    //     "    color = texture(mapKd, fTexCoord);\n"
    //     "}\n";

    // addShader(vsCode, fsCode);
}
void Engine::initlights()
{
    _light_ambient.reset(new AmbientLight());
    _light_ambient->color = {1.0f, 1.0f, 1.0f};
    _light_ambient->intensity = 0.5f;
    _light_directional.reset(new DirectionalLight());
    _light_directional->color = {1.0f, 1.0f, 1.0f};
    _light_directional->intensity = 1.0f;
    _light_directional->transform.rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    _light_point.reset(new PointLight());
    _light_point->color = {1.0f, 1.0f, 1.0f};
    _light_point->intensity = 1.0f;
    _light_point->transform.position = {0.0f, 0.0f, 0.0f};
    _light_point->kc = 1.0f;
    _light_point->kl = 0.0f;
    _light_point->kq = 1.0f;
}
void Engine::cameraRenew()
{
    constexpr float cameraMoveSpeed = 100.0f;
    constexpr float cameraRotateSpeed = 0.02f;

    if (_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE)
    {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    if (_input.keyboard.keyStates[GLFW_KEY_W] != GLFW_RELEASE)
    {
        std::cout << "W" << std::endl;
        _camera->transform.position += _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
        if (_command->_view == 0) {
            shooter->_transform.position += _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
        }
    }

    if (_input.keyboard.keyStates[GLFW_KEY_A] != GLFW_RELEASE)
    {
        std::cout << "A" << std::endl;
        _camera->transform.position -= _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
        if (_command->_view == 0) {
            shooter->_transform.position -= _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
        }
    }

    if (_input.keyboard.keyStates[GLFW_KEY_S] != GLFW_RELEASE)
    {
        std::cout << "S" << std::endl;
        _camera->transform.position -= _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
        if (_command->_view == 0) {
            //std::cout<<"0"<<std::endl;
            shooter->_transform.position -= _camera->transform.getFront() * cameraMoveSpeed * _deltaTime;
        }
    }

    if (_input.keyboard.keyStates[GLFW_KEY_D] != GLFW_RELEASE)
    {
        std::cout << "D" << std::endl;
        _camera->transform.position += _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
        if (_command->_view == 0) {
            shooter->_transform.position += _camera->transform.getRight() * cameraMoveSpeed * _deltaTime;
        }
    }

    if (_input.mouse.move.xNow != _input.mouse.move.xOld)
    {
        std::cout << "mouse move in x direction" << std::endl;
        _camera->transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.xNow + _input.mouse.move.xOld)), glm::vec3(0.0f, 1.0f, 0.0f)) * _camera->transform.rotation;
        if (_command->_view == 0) {
            shooter->_transform.position = _camera->transform.position +
                                           glm::vec3 (3 * _camera->transform.getFront().x, 3* _camera->transform.getFront().y, 3 * _camera->transform.getFront().z)-
                                           glm::vec3 (0.0f, 1.0f, 0.0f);
            shooter->_transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.xNow + _input.mouse.move.xOld)), glm::vec3(0.0f, 1.0f, 0.0f)) * shooter->_transform.rotation;
        }
    }

    if (_input.mouse.move.yNow != _input.mouse.move.yOld)
    {
        std::cout << "mouse move in y direction" << std::endl;
        _camera->transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.yNow + _input.mouse.move.yOld)), _camera->transform.getRight()) * _camera->transform.rotation;
        if (_command->_view == 0) {
            shooter->_transform.position = _camera->transform.position +
                    glm::vec3 (3 * _camera->transform.getFront().x, 3 * _camera->transform.getFront().y, 3 * _camera->transform.getFront().z)-
                    glm::vec3 (0.0f, 1.0f, 0.0f);
            shooter->_transform.rotation = glm::angleAxis(glm::radians(cameraRotateSpeed * (-_input.mouse.move.yNow + _input.mouse.move.yOld)), _camera->transform.getRight()) * shooter->_transform.rotation;
        }
    }
};
