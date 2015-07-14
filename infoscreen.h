#ifndef INFOSCREEN_H
#define INFOSCREEN_H

#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>

class InfoScreen {
private:
    std::string message;
protected:
    bool finished;
public:
    InfoScreen(std::string s);
    virtual void run(sf::RenderWindow &window);
};

#endif // INFOSCREEN_H