#include "infoscreen.h"

InfoScreen::InfoScreen(std::string s) {
    message = s;
    finished = false;
}

void InfoScreen::run(sf::RenderWindow &window) {
    sf::Texture texture;
    texture.loadFromFile("loading_icon.jpg");
    sf::Sprite sprite(texture);
    sf::FloatRect fr = sprite.getGlobalBounds();
    sprite.setOrigin(fr.width / 2.0, fr.height / 2.0);
    sprite.setPosition(window.getSize().x / 2.0, window.getSize().y / 2.0 * 1.25);

    sf::Font font;
    if (!font.loadFromFile("times.ttf")) {
        window.close();
    }
    sf::Text text(message, font, 30);
    double rotation_direction = 1.0;
    double text_max_angle = 9;
    double text_rotation_speed = 0.25;

    text.setOrigin(text.getGlobalBounds().width / 2.0, text.getGlobalBounds().height / 2.0);
    text.setPosition(window.getSize().x / 2.0, window.getSize().y / 2.0 * 0.75);
    text.setColor(sf::Color::Black);

    while (window.isOpen() && !finished) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        text.rotate(rotation_direction * text_rotation_speed);
        if (fmod(360, text.getRotation()) > text_max_angle) {
            rotation_direction *= -1;
            text.rotate(rotation_direction * text_rotation_speed);
        }  

        sprite.rotate(1.0);
        window.clear(sf::Color::White);
        window.draw(text);
        window.draw(sprite);
        window.display();
    }
}