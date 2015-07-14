#include "selectorbox.h"

SelectorBox::SelectorBox() {
    is_black = true;
    shift_pressed = false;
    control_pressed = false;
}

SelectorBox::SelectorBox(sf::Vector2f dim, sf::Vector2f loc, sf::Vector2f window_dim, bool is_black) {
    selector.setSize(dim);
    selector.setPosition(loc);
    this->window_dim = window_dim;
    this->is_black = is_black;
    selector.setFillColor(sf::Color::Transparent);
    if (is_black) {
        selector.setOutlineColor(sf::Color::Black);
    } else {
        selector.setOutlineColor(sf::Color::White);
    }
    selector.setOutlineThickness(1.0);

    movement_vector.push_back(sf::Vector2f(0.0, -1.0)); //up
    movement_vector.push_back(sf::Vector2f(1.0, 0.0));  //right
    movement_vector.push_back(sf::Vector2f(0.0, 1.0));  //down
    movement_vector.push_back(sf::Vector2f(-1.0, 0.0)); //left

    for (int i = UP; i <= LEFT; i++) {
        movement_state.push_back(false);
    }

    speed = 1.0;

    shift_pressed = false;
    control_pressed = false;
}

void SelectorBox::update() {
    //move box
    for (int i = UP; i <= LEFT; i++) {
        if (movement_state[i]) {
            selector.move(movement_vector[i].x * speed, movement_vector[i].y * speed);
        }
    }
    check_bounds();
}

void SelectorBox::check_bounds() {
    //check for out of bounds
    if (selector.getPosition().x < 0) {
        selector.setPosition(0, selector.getPosition().y);
    } else if (selector.getPosition().x + selector.getSize().x > window_dim.x) {
        selector.setPosition(window_dim.x - selector.getSize().x, selector.getPosition().y);
    }
    if (selector.getPosition().y < 0) {
        selector.setPosition(selector.getPosition().x, 0);
    } else if (selector.getPosition().y + selector.getSize().y > window_dim.y) {
        selector.setPosition(selector.getPosition().x, window_dim.y - selector.getSize().y);
    }
}

void SelectorBox::move_with_mouse(sf::Vector2i before, sf::Vector2i after) {
    double dx = after.x - before.x;
    double dy = after.y - before.y;
    //lower sensitivity
    dx /= 2;
    dy /= 2;
    selector.move(dx, dy);
    check_bounds();
}

void SelectorBox::teleport(int loc) {
    switch(loc) {
    case UP:
        selector.setPosition(selector.getPosition().x, 0);
        break;
    case LEFT:
        selector.setPosition(0, selector.getPosition().y);
        break;
    case RIGHT:
        selector.setPosition(window_dim.x - selector.getSize().x, selector.getPosition().y);
        break;
    case DOWN:
        selector.setPosition(selector.getPosition().x, window_dim.y - selector.getSize().y);
        break;
    case CENTER:
        selector.setPosition((window_dim.x - selector.getSize().x) / 2.0, (window_dim.y - selector.getSize().y) / 2.0);
    }
}

//disables all keys
void SelectorBox::deselect_directions() {
    for (int i = UP; i <= LEFT; i++) {
        movement_state[i] = false;
    }
}

void SelectorBox::set_dir(int dir, bool state) {
    movement_state[dir] = state;
    if (state) {
        if (shift_pressed) {
            update();
            movement_state[dir] = false;
        }
        if (control_pressed) {
            teleport(dir);
            movement_state[dir] = false;
        }
    }
}

void SelectorBox::set_shift(bool state) {
    shift_pressed = state;
    if (shift_pressed) {
        deselect_directions();
    }
}

void SelectorBox::set_control(bool state) {
    control_pressed = state;
    if (control_pressed) {
        deselect_directions();
    }
}

void SelectorBox::toggle_color() {
    is_black = !is_black;
    if (is_black) {
        selector.setOutlineColor(sf::Color::Black);
    } else {
        selector.setOutlineColor(sf::Color::White);
    }
}

void SelectorBox::draw(sf::RenderWindow &window) {
    window.draw(selector); 
}

sf::Vector2f SelectorBox::get_size() {
    return selector.getSize();
}

sf::Vector2f SelectorBox::get_loc() {
    return selector.getPosition();
}