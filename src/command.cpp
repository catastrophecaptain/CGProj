#include "command.hpp"
#include "engine.hpp"
#include "shooter.hpp"
Command::Command(Engine *engine) : _engine(engine){

                                   };

Command::~Command() = default;

void Command::handleInput()
{
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_ESCAPE] != GLFW_RELEASE)
    {
        glfwSetWindowShouldClose(_engine->_window, true);
        return;
    }
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_ENTER] != GLFW_RELEASE)
    {
        std::cout << "ENTER" << std::endl;
        if (_engine->_stage == EngineStage::START)
        {
            _engine->_stage = EngineStage::RUN;
        }
    }
    if (_stage_before != _engine->_stage)
    {
        _stage_before = _engine->_stage;
        _engine->shooter->change_stage(_engine->_stage);
    }
    if (_engine->_input.keyboard.keyStates[GLFW_KEY_P] != GLFW_RELEASE)
    {
        unsigned char *data = new unsigned char[3 * _engine->_windowWidth * _engine->_windowHeight];
        glReadPixels(0, 0,_engine->_windowWidth , _engine->_windowHeight, GL_BGR, GL_UNSIGNED_BYTE, data);

        FILE *file;
        static int count = 0;
        std::string filename=std::string(_engine->_assetRootDir)+"screen/"+std::to_string(count)+".bmp";
        fopen_s(&file, filename.c_str(), "wb");
        count++;
        BITMAPFILEHEADER bfheader;
        BITMAPINFOHEADER biheader;

        bfheader.bfType = 'MB'; // BM
        bfheader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 3 * _engine->_windowWidth * _engine->_windowHeight;
        bfheader.bfReserved1 = 0;
        bfheader.bfReserved2 = 0;
        bfheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        biheader.biSize = sizeof(BITMAPINFOHEADER);
        biheader.biWidth = _engine->_windowWidth;
        biheader.biHeight = _engine->_windowHeight;
        biheader.biPlanes = 1;
        biheader.biBitCount = 24; // 24-bit
        biheader.biCompression = BI_RGB; // no compression
        biheader.biSizeImage = 0;
        biheader.biXPelsPerMeter = 100;
        biheader.biYPelsPerMeter = 100;
        biheader.biClrUsed = 0;
        biheader.biClrImportant = 0;

        fwrite(&bfheader, sizeof(BITMAPFILEHEADER), 1, file);
        fwrite(&biheader, sizeof(BITMAPINFOHEADER), 1, file);
        fwrite(data, 3,_engine->_windowWidth * _engine->_windowHeight, file);

        fclose(file);

        delete[] data;
    
    }
        //     if (_engine->_input.mouse.move.xNow != _engine->_input.mouse.move.xOld)
        // {
        //     _transform.rotation = glm::angleAxis(glm::radians(_angle_speed * (-_engine->_input.mouse.move.xNow + _engine->_input.mouse.move.xOld)), glm::vec3(0.0f, 1.0f, 0.0f)) * _transform.rotation;
        // }
        // if (_engine->_input.mouse.move.yNow != _engine->_input.mouse.move.yOld)
        // {
        //     _transform.rotation = glm::angleAxis(glm::radians(_angle_speed * (-_engine->_input.mouse.move.yNow + _engine->_input.mouse.move.yOld)), _transform.getRight()) * _transform.rotation;
        // }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_J] != GLFW_RELEASE)
    {
        _engine->_light_directional->transform.rotation = glm::angleAxis(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * _engine->_light_directional->transform.rotation;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_L] != GLFW_RELEASE)
    {
        _engine->_light_directional->transform.rotation = glm::angleAxis(glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * _engine->_light_directional->transform.rotation;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_I] != GLFW_RELEASE)
    {
        _engine->_light_directional->transform.rotation = glm::angleAxis(glm::radians(1.0f), _engine->_light_directional->transform.getRight()) * _engine->_light_directional->transform.rotation;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_K] != GLFW_RELEASE)
    {
        _engine->_light_directional->transform.rotation = glm::angleAxis(glm::radians(-1.0f), _engine->_light_directional->transform.getRight()) * _engine->_light_directional->transform.rotation;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_5] != GLFW_RELEASE)
    {
        _engine->_light_directional->color[0]-=0.01;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_6] != GLFW_RELEASE)
    {
        _engine->_light_directional->color[0]+=0.01;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_7] != GLFW_RELEASE)
    {
        _engine->_light_directional->color[1]-=0.01;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_8] != GLFW_RELEASE)
    {
        _engine->_light_directional->color[1]+=0.01;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_9] != GLFW_RELEASE)
    {
        _engine->_light_directional->color[2]-=0.01;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_0] != GLFW_RELEASE)
    {
        _engine->_light_directional->color[2]+=0.01;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_MINUS] != GLFW_RELEASE)
    {
        _engine->_light_directional->intensity-=0.01;
    }
    if(_engine->_input.keyboard.keyStates[GLFW_KEY_EQUAL] != GLFW_RELEASE)
    {
        _engine->_light_directional->intensity+=0.01;
    }
}