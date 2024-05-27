#include "engine.hpp"

Options getOptions(int argc, char* argv[]) {
    Options options;
    options.windowTitle = "CSGO";
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowResizable = false;
    options.vSync = true;
    options.msaa = true;
    options.glVersion = {3, 3};
    options.backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    options.assetRootDir = "../media/";

    return options;
}
int main(int argc, char* argv[]) {
    Options options = getOptions(argc, argv);
    Engine engine(options);
    engine.run();
}