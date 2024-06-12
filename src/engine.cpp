#include <engine.hpp>
#include <object.hpp>
#include <command.hpp>
#include <iostream>
#include "ambience.hpp"
#include <base/light.h>
#include <glm/gtc/matrix_transform.hpp>
#include "shooter.hpp"
#include "ghost.hpp"
#include "map.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
};
void Engine::start()
{
    if (0 == PlaySound("../media/sound/Harmonious.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT))
	{
		printf("playsound false");
	}
    musicplaying=true;
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    _input.mouse.move.xNow = _input.mouse.move.xOld = 0.5f * _windowWidth;
    _input.mouse.move.yNow = _input.mouse.move.yOld = 0.5f * _windowHeight;
    glfwSetCursorPos(_window, _input.mouse.move.xNow, _input.mouse.move.yNow);
    _camera.reset(new PerspectiveCamera(
        glm::radians(50.0f), 1.0f * _windowWidth / _windowHeight, 0.1f, 10000.0f));
    _camera->transform.position.z = 10.0f;
    _stage = EngineStage::START;
    initShaders();
    initlights();
    Ambience *ambience = new Ambience(this);
    float shooter_scale = 8.0f;
    glm::vec3 scale = glm::vec3(shooter_scale, shooter_scale, shooter_scale);
    glm::vec3 position = glm::vec3(0.0f, -1.1f, 7.0f);
    float angle = glm::radians(180.0f);
    glm::quat rotation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
    shooter = new Shooter(this, scale, position, rotation);
    int num_ghosts = 10;
    std::uniform_int_distribution<> _num_generator(1, 500);
    std::random_device rd;
    std::mt19937 gen(rd());
    for (int i = 0; i < num_ghosts; i++)
    {
        int x = _num_generator(gen);
        int y = _num_generator(gen);
        int z = _num_generator(gen);
        float ghost_scale = 6.0f;
        glm::vec3 scale(ghost_scale, ghost_scale, ghost_scale);
        glm::vec3 position((x - 250.0f) * 0.8f, fmod(y, 40.0f) + 20.0f, (z - 250.0f) * 2.5f);
        Ghost *ghost = new Ghost(this, scale, position,true);
    }
    // _command->generateGhost();
    Map *map = new Map(this);
    IMGUI_CHECKVERSION(); 
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    startTime = glfwGetTime();
};
void Engine::getCommand()
{
    _input.forwardState();
};
void Engine::renew()
{
    for (auto object : _objects_to_delete)
    {
        deleteObject(object);
        std::cout << "delete object" << std::endl;
    }
    for(auto object:_objects_to_add)
    {
        addObject(object);
    }
    _objects_to_add.clear();
    _objects_to_delete.clear();
    _command->handleInput();
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    for (auto object : _objects)
    {
        object->renew();
    }    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
};
void Engine::check()
{
    for (auto object : _objects)
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
};
void Engine::plot()
{   
    checkFullScreenToggle();
    restart();
    checkMusicToggle();
    for (auto object : _objects)
    {
        if (object->_is_plot)
        {
            object->plot();
        }
    }
    playTime = glfwGetTime() - startTime;
    ImGui::SetNextWindowSize(ImVec2(200, 100)); // 
    ImGui::Begin("Score");                                         
    ImGui::Text("Kill Count: %d", killCount);   
    ImGui::Text("Top Kill Count: %d", topkill);           
    ImGui::Text("Play Time: %f", playTime);  // 这里显示游玩时间，你需要替换playTime为你真实使用的变量     
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
};
bool Engine::checkCollision(Object *fixed, Object *move)
{
    std::vector<Box> boxes = fixed->getBoxs();
    std::vector<Segment> segments = move->getSegments();
    for (auto box : boxes)
    {
        for (auto segment : segments)
        {
            if (checkCollision(box, segment))
            {
                return true;
            }
        }
    }
    return false;
};
bool Engine::checkCollision(Box &box1, Segment &segment2)
{

    glm::vec3 start = segment2.start;
    glm::vec3 end = segment2.end;
    glm::vec3 segment = end - start;
    if (glm::length(segment) < 0.00001f)
    {
        return false;
    }
    Transform transform = box1.transform;
    glm::mat4 model = transform.getLocalMatrix();
    glm::mat4 inv_model = glm::inverse(model);
    glm::vec4 inv_start = inv_model * glm::vec4(start, 1.0f);
    glm::vec4 inv_end = inv_model * glm::vec4(end, 1.0f);
    glm::vec3 dir = glm::normalize(glm::vec3(inv_end - inv_start));

    glm::vec3 inv_dir;
    for (int i = 0; i < 3; i++)
    {
        if (dir[i] == 0.0f)
        {
            inv_dir[i] = std::numeric_limits<float>::max() / 100;
        }
        else
        {
            inv_dir[i] = 1.0f / dir[i];
        }
    }

    glm::vec3 t1 = (box1.min - glm::vec3(inv_start)) * inv_dir;
    glm::vec3 t2 = (box1.max - glm::vec3(inv_start)) * inv_dir;
    glm::vec3 tmin = glm::min(t1, t2);
    glm::vec3 tmax = glm::max(t1, t2);
    float t_enter = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
    float t_exit = glm::min(glm::min(tmax.x, tmax.y), tmax.z);
    _t_min = t_enter / glm::length(glm::vec3(transform.rotation * dir));
    if (t_enter <= t_exit && t_enter < glm::length(glm::vec3(inv_end - inv_start)) && t_exit > 0)
    {
        return true;
    }
    return false;
}
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
            delete object;
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
        uniform vec3 scale;
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
            color = vec4(scale*(ambient + diffuse_specular), 1.0f);

            
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
    const char *segment_vsCode = R"glsl(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            void main()
            {
               gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
            }
            )glsl";

    const char *segment_fsCode = R"glsl(
            #version 330 core
            out vec4 FragColor;
            void main()
            {
               FragColor = vec4(1.0, 0.5, 0.2, 1.0);
            }
            )glsl";
    addShader(segment_vsCode,segment_fsCode);
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
    _light_point->intensity = 400000.0f;
    _light_point->transform.position = {0.0f, 1000.0f, 100.0f};
    _light_point->kc = 1.0f;
    _light_point->kl = 0.0f;
    _light_point->kq = 1.0f;
}
void Engine::checkFullScreenToggle()
{
    if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_RELEASE){
        kKeyReleased=true;
    }
    else if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS&&kKeyReleased)
    {
        // F key has been pressed, toggle full screen mode
        toggleFullScreen();
        kKeyReleased=false;
    }
}
void Engine::toggleFullScreen()
{
    static int savedWidth = 0, savedHeight = 0, savedPosX = 0, savedPosY = 0;  
    static int isFullscreen = 0;

    if (!isFullscreen)
    {
        // Store the old window size and position
        glfwGetWindowSize(_window, &savedWidth, &savedHeight);
        glfwGetWindowPos(_window, &savedPosX, &savedPosY);

        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        // Set the window size to the monitor (full screen)
        glfwSetWindowMonitor(_window, glfwGetPrimaryMonitor(), 0, 0,
                             mode->width, mode->height, mode->refreshRate);

        isFullscreen = 1;
    }
    else
    {
        // restore old window size and position
        glfwSetWindowMonitor(_window, NULL, savedPosX, savedPosY,
                             savedWidth, savedHeight, 0);
        isFullscreen = 0;
    }
}
void Engine::restart(){
    if(glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS&&_stage==EngineStage::END){
        _stage=EngineStage::START;
        shooter->change_stage(EngineStage::START);
        if(killCount>topkill)topkill=killCount;
        killCount=0;
    }
}
void Engine::toggleMusic()
{
    if (musicplaying) 
    {
        PlaySound(NULL, 0, 0);
        musicplaying = false;
    } 
    else 
    {
        PlaySound("../media/sound/Harmonious.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
        musicplaying = true;
    }
}
void Engine::checkMusicToggle()
{
    if (glfwGetKey(_window, GLFW_KEY_M) == GLFW_RELEASE)
    {
         mKeyReleased = true;
    }
    
    if (mKeyReleased && glfwGetKey(_window, GLFW_KEY_M) == GLFW_PRESS)
    {
        toggleMusic();
        mKeyReleased = false;
    }
}