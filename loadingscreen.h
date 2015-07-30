#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <SFML/Graphics.hpp>

#include "infoscreen.h"

//call python to get file list
#ifdef SYS_WINDOWS
const std::string PYTHON_PROCESS_IMAGES = "py wallinfo.py";
#else //unix systems
const std::string PYTHON_PROCESS_IMAGES = "python wallinfo.py";
#endif

class LoadingScreen : public InfoScreen {
private:
    void process_images();
public:
    LoadingScreen(std::string s);
    void run(sf::RenderWindow &window);
};

#endif // LOADINGSCREEN_H