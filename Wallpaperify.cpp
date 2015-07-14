#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "infoscreen.h"
#include "loadingscreen.h"
#include "imagemanipulation.h"

const std::string VERSION = "0.5.0";

bool check_if_file_exists(std::string fname);
void write_screen_resolution(int &screen_width, int &screen_height);
int assert_wallpaper_directory();
void position_text(sf::Vector2f loc, std::string str, sf::Text &text);
void display_text(sf::RenderWindow &window, sf::Text text);

int main(int argc, char** argv) {
    int screen_width, screen_height;
    write_screen_resolution(screen_width, screen_height);
    const int WINDOW_WIDTH = screen_width * 0.85;
    const int WINDOW_HEIGHT = screen_height * 0.85;

    int directory_exist = assert_wallpaper_directory();

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(500, 500), "Wallpaperifier by Mitsuru Otsuka",
                            sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(120);
    window.setKeyRepeatEnabled(false);

    sf::Font font;
    font.loadFromFile("media/times.ttf");
    sf::Text text("", font, 30);
    if (directory_exist != 0) {
        std::string fail_str;
        if (directory_exist == 1) {
            fail_str = "Error Code 1: No path found. See instructions.txt for more details.";
        } else {
            fail_str = "Error Code 2: Invalid directory path. See instructions.txt for more details.";
        }
        fail_str += "\nPress Q or ESC to exit the program";
        position_text(sf::Vector2f(25.0, 25.0), fail_str, text);
        display_text(window, text);
    } else {
        //Check images, show loading screen while checking
        LoadingScreen loadingscreen("Processing Images... Please wait Nyaa~");
        loadingscreen.run(window);

        if (check_if_file_exists("toresize.txt")) {
            //start image manipulation program
            ImageManipulation imagemanipulation(screen_width, screen_height);
            imagemanipulation.run(window);
        } else {
            std::string nothing_to_change = "There are no more images that need to be changed.";
            nothing_to_change += "\nPress Q or ESC to exit the program";
            position_text(sf::Vector2f(WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0), nothing_to_change, text);
            display_text(window, text);
        }
    }
    InfoScreen infoscreen("All Images Finished Processing Nyaa~");
    infoscreen.run(window);
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
        return check_if_file_exists(wallpaper_location) ? 0 : 2;
    }
    return 1;
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
                if (e.key.code == sf::Keyboard::Q || e.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        window.clear(sf::Color::White);
        window.draw(text);
        window.display();
    }
}