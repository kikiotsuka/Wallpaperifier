#include "imagemanipulation.h"

ImageManipulation::ImageManipulation(int s_width, int s_height) {
    screen_dim = sf::Vector2f(s_width, s_height);
    target_dim = sf::Vector2f(s_width * SCREEN_FRACTION, s_height * SCREEN_FRACTION);
    window_aspect_ratio = screen_dim.x / screen_dim.y;

    std::cout << "===Screen Meta===" << "\n";
    std::cout << "Screen Dim: (" << screen_dim.x << ", " << screen_dim.y << ")" << "\n";
    std::cout << "Window Dim: (" << target_dim.x << ", " << target_dim.y << ")" << "\n";
    std::cout << "Window Aspect Ratio: " << window_aspect_ratio << "\n";

    finalize = false;
    is_black = true;
    readtochangelist();

    for (int i = 0; i < 2; i++) {
        preview_box[i].setOutlineThickness(0);
    }
}

//TODO
/*
    selector box
    fill in color box
*/
void ImageManipulation::run(sf::RenderWindow &window) {
    while (window.isOpen() && !imgnames.empty()) {
        std::string current = imgnames.front();
        std::cout << current << "\n";
        imgnames.pop();   
        texture.loadFromFile(current);
        texture.setSmooth(true);
        sprite.setTexture(texture, true);

        //reset variables
        finalize = false;
        is_black = true;

        int image_status = IMAGE_SUCCESS;
        int force_mode = 0;
        //if force mode is 1, run crop_image
        //if force mode is 2, run minimalistify_image
        sf::Vector2f sprite_dim(sprite.getGlobalBounds().height, sprite.getGlobalBounds().width);
        do {
            if (force_mode == 1 || (force_mode == 0 && sprite_dim.x < sprite_dim.y)) {
                image_status = crop_image(window, current);
                if (image_status == IMAGE_SWITCH) {
                    force_mode = 2;
                }
            } else if (force_mode == 2 || force_mode == 0) {
                //if the sprite is taller than wide, initialize minimal mode
                image_status = minimalistify_image(window, current);
                if (image_status == IMAGE_SWITCH) {
                    force_mode = 1;
                }
            }
            //push image back into queue
            if (image_status == IMAGE_SKIP) {
                imgnames.push(current);
                image_status = IMAGE_SUCCESS;
            } else if (image_status == IMAGE_FAILURE) {
                std::cout << "FATAL ERROR" << "\n";
            }
        } while (image_status != IMAGE_SUCCESS);
    }

    //reset the window after finished
    reset_window(window, 500, 500);
}

//Pragma to surpress warnings about unhandled enumeration values in switch statement in clang
#pragma clang diagnostic ignored "-Wswitch"

//Function returns true on success, false when user wishes to skip image
int ImageManipulation::crop_image(sf::RenderWindow &window, std::string fname) {
    int image_status = IMAGE_SUCCESS;

    double horz_scale = target_dim.x / texture.getSize().x;
    double vert_scale = target_dim.y / texture.getSize().y;
    double selected_scale = horz_scale;
    orientation = ORIENTATION_HORIZONTAL;
    /*
        We take the scale with the smaller value because it is the bigger dimension in this case
    */
    if (vert_scale < horz_scale) {
        selected_scale = vert_scale;
        orientation = ORIENTATION_VERTICAL;
    }

    std::cout << "Candidate Scales; Horz: " << horz_scale << "; Vert: " << vert_scale << "\n";
    std::cout << "Selected Scale: " << selected_scale << "\n";
    std::cout << "Original Dimension: (" << sprite.getGlobalBounds().width << ", " << 
        sprite.getGlobalBounds().height << ")" << "\n";

    sprite.setScale(selected_scale, selected_scale);
    reset_window(window, sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);

    std::cout << "After Dimension: (" << sprite.getGlobalBounds().width << ", " <<
        sprite.getGlobalBounds().height << ")" << "\n";

    initialize_selector();

    bool working = true;
    while (window.isOpen() && working) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                if (!finalize) {
                    switch (e.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::W:
                    case sf::Keyboard::Up:
                        selector.set_dir(UP, true);
                        break;
                    case sf::Keyboard::S:
                    case sf::Keyboard::Down:
                        selector.set_dir(DOWN, true);
                        break;
                    case sf::Keyboard::D:
                    case sf::Keyboard::Right:
                        selector.set_dir(RIGHT, true);
                        break;
                    case sf::Keyboard::A:
                    case sf::Keyboard::Left:
                        selector.set_dir(LEFT, true);
                        break;
                    case sf::Keyboard::Numpad8:
                        selector.teleport(UP);
                        break;
                    case sf::Keyboard::Numpad2:
                        selector.teleport(DOWN);
                        break;
                    case sf::Keyboard::Numpad4:
                        selector.teleport(LEFT);
                        break;
                    case sf::Keyboard::Numpad6:
                        selector.teleport(RIGHT);
                        break;
                    case sf::Keyboard::Numpad5:
                    case sf::Keyboard::Num5:
                        selector.teleport(CENTER);
                        break;
                    case sf::Keyboard::Space:
                        is_black = !is_black;
                        selector.toggle_color();
                        break;
                    case sf::Keyboard::C:
                        image_status = IMAGE_SWITCH;
                        working = false;
                        break;
                    case sf::Keyboard::P:
                        image_status = IMAGE_SKIP;
                        working = false;
                        break;
                    case sf::Keyboard::LShift:
                    case sf::Keyboard::RShift:
                        selector.set_shift(true);
                        break;
                    /*
                    //Because pressing control key on Arch doesn't work
                    case sf::Keyboard::LControl:
                    case sf::Keyboard::RControl:
                        std::cout << "CONTROL KEY WAS PRESSED" << "\n";
                        break;
                    */
                    case sf::Keyboard::Return:
                        finalize = true;
                        init_preview_box();
                        break;
                    }
                } else {
                    if (e.key.code == sf::Keyboard::Return) {
                        if (!generate_image(fname, vert_scale + horz_scale - selected_scale)) {
                            image_status = IMAGE_FAILURE;
                        }
                        working = false;
                    } else if (e.key.code == sf::Keyboard::Escape) {
                        finalize = false;
                    } else if (e.key.code == sf::Keyboard::Space) {
                        is_black = !is_black;
                        init_preview_box();
                        selector.toggle_color();
                    }
                }
            } else if (e.type == sf::Event::KeyReleased) {
                switch (e.key.code) {
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    selector.set_dir(UP, false);
                    break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    selector.set_dir(DOWN, false);
                    break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    selector.set_dir(RIGHT, false);
                    break;
                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    selector.set_dir(LEFT, false);
                    break;
                case sf::Keyboard::LShift:
                case sf::Keyboard::RShift:
                    selector.set_shift(false);
                    break;
                /*
                //For some reason pressing control keys doesn't work on Arch
                case sf::Keyboard::LControl:
                case sf::Keyboard::RControl:
                    std::cout << "CONTROL KEY WAS RELEASED" << "\n";
                    break;
                */
                }
            } else if (e.type == sf::Event::MouseButtonPressed) {
                if (e.mouseButton.button == sf::Mouse::Left) {
                    mouse_down = true;
                    movement.first = sf::Vector2i(e.mouseButton.x, e.mouseButton.y);
                    movement.second = movement.first;
                } 
            } else if (e.type == sf::Event::MouseButtonReleased) {
                if (e.mouseButton.button == sf::Mouse::Left) {
                    mouse_down = false;
                    movement.first = sf::Vector2i();
                    movement.second = sf::Vector2i();
                }
            } else if (e.type == sf::Event::MouseMoved) {
                if (mouse_down) {
                    movement.first = movement.second;
                    movement.second = sf::Vector2i(e.mouseMove.x, e.mouseMove.y);
                    selector.move_with_mouse(movement.first, movement.second);
                }
            }
        }
        //because pressing control doesn't trigger an event unless you do shift + control
        //at least on Arch
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || 
            sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
            selector.set_control(true);
        } else {
            selector.set_control(false);
        }

        selector.update();

        window.clear(sf::Color::White);
        window.draw(sprite);
        selector.draw(window);
        if (finalize) {
            for (int i = 0; i < 2; i++) {
                window.draw(preview_box[i]);
            }
        }
        window.display();
    }
    return image_status;
}

//Function returns true on success, false when user wishes to skip image
//TODO finish me
int ImageManipulation::minimalistify_image(sf::RenderWindow &window, std::string fname) {
    int image_status = IMAGE_SUCCESS;
    std::cout << "Minimalist mode" << "\n";
    return image_status;
}

/*
    screen_width / screen_height = new_image_width / new_image_height
*/
void ImageManipulation::initialize_selector() {
    sf::Vector2f selector_dim, selector_pos;
    if (orientation == ORIENTATION_HORIZONTAL) {
        double sprite_height = sprite.getGlobalBounds().height;
        selector_dim = sf::Vector2f(window_aspect_ratio * sprite_height, sprite_height);
        selector_pos = sf::Vector2f((target_dim.x - selector_dim.x) / 2.0, 0);
    } else {
        double sprite_width = sprite.getGlobalBounds().width;
        selector_dim = sf::Vector2f(sprite_width, sprite_width / window_aspect_ratio);
        selector_pos = sf::Vector2f(0, (target_dim.y - selector_dim.y) / 2.0);
    } 
    sf::Vector2f img_dim(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
    selector = SelectorBox(selector_dim, selector_pos, img_dim, is_black);
}

void ImageManipulation::init_preview_box() {
    sf::Vector2f sprite_dim(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
    if (orientation == ORIENTATION_HORIZONTAL) {
        preview_box[0].setSize(sf::Vector2f(selector.get_loc().x, sprite_dim.y));
        preview_box[0].setPosition(0, 0);
        preview_box[1].setSize(sf::Vector2f(sprite_dim.x - (selector.get_loc().x + selector.get_size().x),
                                            sprite_dim.y));
        preview_box[1].setPosition(selector.get_loc().x + selector.get_size().x, 0);
    } else {
        preview_box[0].setSize(sf::Vector2f(sprite_dim.x, selector.get_loc().y));
        preview_box[0].setPosition(0, 0);
        preview_box[1].setSize(sf::Vector2f(sprite_dim.x, 
                                            sprite_dim.y - (selector.get_loc().y + selector.get_size().y)));
        preview_box[1].setPosition(0, selector.get_loc().y + selector.get_size().y);
    }
    sf::Color fill_color = sf::Color::Black;
    if (!is_black) {
        fill_color = sf::Color::White;
    }
    for (int i = 0; i < 2; i++) {
        preview_box[i].setFillColor(fill_color);
    }
}

bool ImageManipulation::generate_image(std::string imgname, double unusedscale) {
    double screen_scale = unusedscale / SCREEN_FRACTION;
    std::cout << "Selector coord: (" << selector.get_loc().x << ", " << selector.get_loc().y << ")\n";
    std::cout << "Selector size: (" << selector.get_size().x << ", " << selector.get_size().y << ")\n";
    sprite.setScale(screen_scale, screen_scale);
    sprite.setPosition(-selector.get_loc().x * screen_scale, -selector.get_loc().y * screen_scale);
    sf::RenderTexture rt;
    rt.create(screen_dim.x, screen_dim.y);
    rt.setSmooth(true);
    rt.draw(sprite);
    rt.display();
    std::cout << sprite.getGlobalBounds().width << " " << sprite.getGlobalBounds().height << "\n";
    std::cout << sprite.getPosition().x << " " << sprite.getPosition().y << "\n";
    std::cout << unusedscale << " " << screen_scale << "\n";
    return write_image(imgname, rt.getTexture());
}

bool ImageManipulation::write_image(std::string imgname, const sf::Texture &t) {
    sf::Image img = t.copyToImage();
    return img.saveToFile(imgname);
}

void ImageManipulation::readtochangelist() {
    std::ifstream fin("toresize.txt");
    std::string s;
    while (getline(fin, s) && s != "") {
        imgnames.push(s);
   }
}

void ImageManipulation::reset_window(sf::RenderWindow &window, int w_width, int w_height) {
    window.setSize(sf::Vector2u(w_width, w_height));
    window.setPosition(sf::Vector2i((screen_dim.x - w_width) / 2.0, (screen_dim.y - w_height) / 2.0));
    sf::View view(sf::FloatRect(0, 0, w_width, w_height));
    window.setView(view);
}