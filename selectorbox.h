#ifndef SELECTORBOX_H
#define SELECTORBOX_H

#include <SFML/Graphics.hpp>
#include <vector>

const int UP = 0;
const int RIGHT = 1;
const int DOWN = 2;
const int LEFT = 3;
const int CENTER = 4;

class SelectorBox {
private:
    sf::RectangleShape selector;
    bool is_black;
    double speed;

    std::vector<sf::Vector2f> movement_vector;
    std::vector<bool> movement_state;
    //if shift is pressed, image moves one pixel
    bool shift_pressed;
    //if ctrl is pressed, teleport instead of move
    bool control_pressed;

    sf::Vector2f window_dim;
public:
    SelectorBox();
    SelectorBox(sf::Vector2f dim,
                sf::Vector2f loc, 
                sf::Vector2f screen_dim, 
                bool is_black);
    void update();
    void check_bounds();
    void move_with_mouse(sf::Vector2i before, 
                         sf::Vector2i after);
    void teleport(int loc);
    void deselect_directions();
    void set_dir(int dir, bool state);
    void set_shift(bool state);
    void set_control(bool state);
    void set_outline_width(double width);
    void toggle_color();
    void draw(sf::RenderWindow &window);
    sf::Vector2f get_size();
    sf::Vector2f get_loc();
};

#endif // SELECTORBOX_H