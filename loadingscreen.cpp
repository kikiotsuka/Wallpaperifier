#include "loadingscreen.h"

LoadingScreen::LoadingScreen(std::string s) : InfoScreen(s) {

}

void LoadingScreen::run(sf::RenderWindow &window) {
    sf::Thread thread(&LoadingScreen::process_images, this); 
    thread.launch();

    InfoScreen::run(window);
}

void LoadingScreen::process_images() {
    system(PYTHON_PROCESS_IMAGES.c_str());
    finished = true;
}