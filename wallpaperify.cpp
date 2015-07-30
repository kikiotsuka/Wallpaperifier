#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "infoscreen.h"
#include "loadingscreen.h"
#include "imagemanipulation.h"

extern const std::string VERSION = "1.3.0";
extern const std::string TITLE = "Wallpaperify Version " + VERSION + " by Mitsuru Otsuka";

const int DIRECTORY_EXISTS = 0;
const int DIRECTORY_INVALID = 1;

const std::string PYTHON_SELECT_DIR = "python wallfolderselector.py";

bool check_if_file_exists(std::string fname);
void write_screen_resolution(int &screen_width, int &screen_height);
int assert_wallpaper_directory();
void position_text(sf::Vector2f loc, std::string str, sf::Text &text);
void display_text(sf::RenderWindow &window, sf::Text text);

int main(int argc, char** argv) {
    //check if wallpaper folder exists
    int directory_exist = assert_wallpaper_directory();
    //if it doesn't exist or is invalid
    if (directory_exist == DIRECTORY_INVALID) {
        system(PYTHON_SELECT_DIR.c_str());
    }

    //check if the python call succeeded
    directory_exist = assert_wallpaper_directory();
    bool success = directory_exist == DIRECTORY_EXISTS ? true : false;

    int screen_width, screen_height;
    write_screen_resolution(screen_width, screen_height);
    const int WINDOW_WIDTH = screen_width * SCREEN_FRACTION;
    const int WINDOW_HEIGHT = screen_height * SCREEN_FRACTION;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(500, 500), TITLE, sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(120);
    window.setKeyRepeatEnabled(false);

    if (success) {
        //Check images, show loading screen while checking
        LoadingScreen loadingscreen("Processing Images... Please wait Nyaa~");
        loadingscreen.run(window);

        //start image manipulation program
        ImageManipulation imagemanipulation(screen_width, screen_height);
        imagemanipulation.run(window);

        InfoScreen infoscreen("All Images Finished Processing Nyaa~");
        infoscreen.run(window);
    } else {
        sf::Font font;
        font.loadFromFile("media/times.ttf");
        sf::Text text("", font, 23);
        std::string error_text = "Error: Invalid directory, try restarting the program\nPress ESC to exit";
        position_text(sf::Vector2f(250, 250), error_text, text);
        display_text(window, text);
    }
}

bool check_if_file_exists(std::string fname) {
    std::ifstream test(fname.c_str());
    return test.good(); 
}

//Write the screen resolution to a file
void write_screen_resolution(int &screen_width, int &screen_height) {
    std::ofstream fscreen("screenres.txt");
    sf::VideoMode dim = sf::VideoMode::getDesktopMode();
    screen_width = dim.width;
    screen_height = dim.height;
    fscreen << screen_width << "\n" << screen_height << "\n";
    fscreen.close();
}

//Returns true if the wallpaper directory exists and is valid
int assert_wallpaper_directory() {
    if (check_if_file_exists("wallpaperlocation.txt")) {
        std::string wallpaper_location;
        std::ifstream test_exist("wallpaperlocation.txt");
        getline(test_exist, wallpaper_location);
        return check_if_file_exists(wallpaper_location) ? DIRECTORY_EXISTS : DIRECTORY_INVALID;
    }
    return DIRECTORY_INVALID;
}

void position_text(sf::Vector2f loc, std::string str, sf::Text &text) {
    text.setString(str);
    sf::FloatRect fr = text.getGlobalBounds(); 
    text.setPosition(loc.x - fr.width / 2.0, loc.y - fr.height / 2.0);
    text.setColor(sf::Color::Black);
}

void display_text(sf::RenderWindow &window, sf::Text text) { 
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        window.clear(sf::Color::White);
        window.draw(text);
        window.display();
    }
}