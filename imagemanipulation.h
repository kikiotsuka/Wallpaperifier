#ifndef IMAGEMANIPULATION_H
#define IMAGEMANIPULATION_H

#include <SFML/Graphics.hpp>
#include <queue>
#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <fstream>
#include <cstdio>
#include <cmath>

#include "selectorbox.h"

const int ORIENTATION_HORIZONTAL = 0;
const int ORIENTATION_VERTICAL = 1;

const double SCREEN_FRACTION = 0.85;

const int IMAGE_SUCCESS = 0;
const int IMAGE_SWITCH = 1;
const int IMAGE_SKIP = 2;
const int IMAGE_FAILURE = 3;

const int MODE_CROP = 0;
const int MODE_MINIMALISTIC = 1;

const double EPSILON = 10e-5;

extern const std::string TITLE;

class ImageManipulation {
private:
    SelectorBox selector;
    sf::RectangleShape preview_box[2];
    int orientation;
    bool finalize;

    //mouse event stuff
    bool mouse_down;
    std::pair<sf::Vector2i, sf::Vector2i> movement;

    bool is_black;
    bool delete_image;

    std::queue<std::string> imgnames;
    //target_dim is screen_dim * SCREEN_FRACTION
    sf::Vector2f screen_dim, target_dim;
    double window_aspect_ratio;
    sf::Texture texture;
    sf::Sprite sprite;

    int crop_image(sf::RenderWindow &window, std::string fname);
    int minimalistify_image(sf::RenderWindow &window, std::string fname);

    bool main_keyboard_pressed_input(sf::Event e,
                                     sf::RenderWindow &window,
                                     int &image_status,
                                     bool &working,
                                     bool &finalize);
    bool main_keyboard_released_input(sf::Event e);
    void initialize_selector(int mode);

    void init_preview_box();
    bool generate_crop_image(std::string imgname, double unusedscale);
    bool generate_minimalist_image(std::string imgname, sf::Color fill_color);
    bool write_image(std::string imgname, const sf::Texture &t);

    void readtochangelist();
    void position_text(sf::Vector2f loc, 
            std::string str, sf::Text &text);
    void reset_window(sf::RenderWindow &window,
                      int w_width, int w_height);
public:
    ImageManipulation(int s_width, int s_height);
    void run(sf::RenderWindow &window);
};

#endif // IMAGEMANIPULATION_H