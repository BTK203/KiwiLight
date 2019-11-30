#include "KiwiLight.h"

/**
 * 3695 thing here
 */

using namespace cv;
using namespace KiwiLight;

int main(int argc, char *argv[]) {
    if(argc == 1) {
        KiwiLightApp::Create(argc, argv);
        KiwiLightApp::Start();
    } else {
        std::cout << "Command: \"" << argv[1] << "\"" << std::endl;
        //special command, do something else
        if(std::string(argv[1]) == "-c") {
            std::string configPath = argv[2];
            std::cout << "Config Path: " << configPath << std::endl;
            //create the KiwiLight runner and camera
            KiwiLightApp::OpenNewCameraOnIndex(0);
            Runner runner = Runner(configPath, true);
            runner.Loop();
        }
    }
    
    return 0;
}
