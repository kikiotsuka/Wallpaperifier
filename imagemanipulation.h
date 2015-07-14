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

#include "selectorbox.h"

const int ORIENTATION_HORIZONTAL = 0;
const int ORIENTATION_VERTICAL = 1;

const double SCREEN_FRACTION = 0.85;

const int IMAGE_SUCCESS = 0;
const int IMAGE_SWITCH = 1;
const int IMAGE_SKIP = 2;
const int IMAGE_FAILURE = 3;

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

    std::queue<std::string> imgnames;
    //target_dim is screen_dim * SCREEN_FRACTION
    sf::Vector2f screen_dim, target_dim;
    double window_aspect_ratio;
    sf::Texture texture;
    sf::Sprite sprite;

    int crop_image(sf::RenderWindow &window, std::string fname);
    int minimalistify_image(sf::RenderWindow &window, std::string fname);

    void initialize_selector();

    void init_preview_box();
    bool generate_image(std::string imgname, double unusedscale);
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