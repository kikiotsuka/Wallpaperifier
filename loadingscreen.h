#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <SFML/Graphics.hpp>

#include "infoscreen.h"

const std::string PYTHON_PROCESS_IMAGES = "python wallinfo.py";

class LoadingScreen : public InfoScreen {
private:
    void process_images();
public:
    LoadingScreen(std::string s);
    void run(sf::RenderWindow &window);
};

#endif // LOADINGSCREEN_H