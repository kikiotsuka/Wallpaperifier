#include "imagemanipulation.h"

/*
    TODO
        - Implement deleting images with a confirmation thingy
        - Implement resizing image to max height dim in minimalistic mode
*/

ImageManipulation::ImageManipulation(int s_width, int s_height) {
    screen_dim = sf::Vector2f(s_width, s_height);
    target_dim = sf::Vector2f(s_width * SCREEN_FRACTION, s_height * SCREEN_FRACTION);
    window_aspect_ratio = screen_dim.x / screen_dim.y;

    finalize = false;
    is_black = true;
    mouse_down = false;
    readtochangelist();
}

void ImageManipulation::run(sf::RenderWindow &window) {
    while (window.isOpen() && !imgnames.empty()) {
        std::string current = imgnames.front();
        const size_t dot_pos = imgnames.rfind('.');
        std::string imgname_without_path = imgnames.substr(dot_pos + 1);
        window.setTitle(TITLE + " - " + imgname_without_path);
        imgnames.pop();   
        texture.loadFromFile(current);
        texture.setSmooth(true);
    
        int image_status = IMAGE_SUCCESS;
        int force_mode = 0;
        //if force mode is 1, run crop_image
        //if force mode is 2, run minimalistify_image
        do {
            sprite.setPosition(0, 0);
            sprite.setScale(1.0, 1.0);
            sprite.setTexture(texture, true);

            //reset variables
            finalize = false;
            is_black = true;
            mouse_down = false;

            sf::Vector2f sprite_dim(sprite.getGlobalBounds().height, sprite.getGlobalBounds().width);

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
    //initialize preview boxes
    for (int i = 0; i < 2; i++) {
        preview_box[i].setOutlineThickness(0);
    }

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

    sprite.setScale(selected_scale, selected_scale);
    reset_window(window, sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);

    initialize_selector(MODE_CROP);

    bool working = true;

    //check if the selected image already has the correct aspect ratio for auto resizing
    double diff = window_aspect_ratio - (sprite.getGlobalBounds().width / sprite.getGlobalBounds().height);
    if (fabs(diff) < EPSILON) {
        working = false;
        if (!generate_crop_image(fname, vert_scale + horz_scale - selected_scale)) {
            image_status = IMAGE_FAILURE;
        }
    }

    while (window.isOpen() && working) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                if (!finalize) {
                    main_keyboard_pressed_input(e, window, image_status, working, finalize);
                } else {
                    if (e.key.code == sf::Keyboard::Return) {
                        if (!generate_crop_image(fname, vert_scale + horz_scale - selected_scale)) {
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
                main_keyboard_released_input(e);
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

int ImageManipulation::minimalistify_image(sf::RenderWindow &window, std::string fname) {
    int image_status = IMAGE_SUCCESS;
    //check if the image is bigger than the window, if so resize it
    if (sprite.getGlobalBounds().height > target_dim.y) {
        double vert_scale = target_dim.y / sprite.getGlobalBounds().height;
        sprite.setScale(vert_scale, vert_scale);
    }

    reset_window(window, target_dim.x, target_dim.y);

    //use to obtain pixel color at coordinate
    sf::RenderTexture rt;
    rt.create(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
    rt.setSmooth(true);
    rt.draw(sprite);
    rt.display();
    sf::Image img = rt.getTexture().copyToImage();

    initialize_selector(MODE_MINIMALISTIC);

    bool key_f = false;
    sf::Color fill_color = sf::Color::White;

    bool working = true;
    while (window.isOpen() && working) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            } else if (e.type == sf::Event::KeyPressed) {
                if (!finalize && !key_f) {
                    //check if input is triggered, otherwise check rest of cases specific to minimalistic mode
                    if (!main_keyboard_pressed_input(e, window, image_status, working, finalize)) {
                        switch (e.key.code) {
                        case sf::Keyboard::F:
                            key_f = true;
                            break;
                        case sf::Keyboard::BackSpace:
                            if (key_f) {
                                fill_color = sf::Color::White;
                            }
                            break;
                        }
                    }
                } else if (finalize) {
                    if (e.key.code == sf::Keyboard::Return) {
                        if (!generate_minimalist_image(fname, fill_color)) {
                            image_status = IMAGE_FAILURE;
                        }
                        working = false;
                    } else if (e.key.code == sf::Keyboard::Escape) {
                        finalize = false;
                    }
                } else if (key_f) {
                    key_f = false;
                    fill_color = sf::Color::White;
                }
            } else if (e.type == sf::Event::KeyReleased) {
                main_keyboard_released_input(e);
            } else if (e.type == sf::Event::MouseButtonPressed) {
                if (e.mouseButton.button == sf::Mouse::Left) {
                    if (!key_f) {
                        mouse_down = true;
                        movement.first = sf::Vector2i(e.mouseButton.x, e.mouseButton.y);
                        movement.second = movement.first;
                    } else {
                        key_f = false;
                    }
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
                } else if (key_f) {
                    sf::Vector2i pixel(e.mouseMove.x - selector.get_loc().x, e.mouseMove.y - selector.get_loc().y);
                    if (pixel.x < 0 || pixel.y < 0 ||
                        pixel.x > sprite.getGlobalBounds().width || pixel.y > sprite.getGlobalBounds().height) {
                        fill_color = sf::Color::White;
                    } else {
                        fill_color = img.getPixel(pixel.x, pixel.y);
                    }
                }
            }
            
            //because pressing control doesn't trigger an event unless you do shift + control
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || 
                sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                selector.set_control(true);
            } else {
                selector.set_control(false);
            }
        }
        selector.update();
        sprite.setPosition(selector.get_loc());

        window.clear(fill_color);
        window.draw(sprite);
        selector.draw(window);
        window.display();
    }

    return image_status;
}

/*
    Accepts user input and executes a task according to the input.
    This function exists to remove duplicate input checks between crop/minimalist mode
    Returns true if an input is triggered, false otherwise
*/
bool ImageManipulation::main_keyboard_pressed_input(sf::Event e, sf::RenderWindow &window, 
                                                    int &image_status, bool &working, bool &finalize) {
    switch(e.key.code) {
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
    case sf::Keyboard::C:
        image_status = IMAGE_SWITCH;
        working = false;
        break;
    case sf::Keyboard::P:
        image_status = IMAGE_SKIP;
        working = false;
        break;
    case sf::Keyboard::R:
        if (sprite.getTextureRect().left != 0) {
            sprite.setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
        } else {
            sprite.setTextureRect(sf::IntRect(texture.getSize().x, 0, -texture.getSize().x, texture.getSize().y)); 
        }
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
        break;
    default:
        return false;
    }
    return true;
}

bool ImageManipulation::main_keyboard_released_input(sf::Event e) {
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
    default:
        return false;
    }
    return true;
}

/*
    screen_width / screen_height = new_image_width / new_image_height
*/
void ImageManipulation::initialize_selector(int mode) {
    if (mode == MODE_CROP) {
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
    } else {
        sf::Vector2f selector_dim(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
        sf::Vector2f selector_pos(target_dim.x - selector_dim.x, target_dim.y - selector_dim.y);

        selector = SelectorBox(selector_dim, selector_pos, target_dim, is_black);
        selector.set_outline_width(0);
        sprite.setPosition(selector.get_loc());
    }
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

bool ImageManipulation::generate_crop_image(std::string imgname, double unusedscale) {
    double screen_scale = unusedscale / SCREEN_FRACTION;
    sprite.setScale(screen_scale, screen_scale);
    sprite.setPosition(-selector.get_loc().x * screen_scale, -selector.get_loc().y * screen_scale);
    sf::RenderTexture rt;
    rt.create(screen_dim.x, screen_dim.y);
    rt.setSmooth(true);
    rt.draw(sprite);
    rt.display();
    return write_image(imgname, rt.getTexture());
}

bool ImageManipulation::generate_minimalist_image(std::string imgname, sf::Color fill_color) {
    double screen_scale = screen_dim.x / target_dim.x;
    //Note the use of scale instead of setScale, the scale is relative to the current scale
    sprite.scale(screen_scale, screen_scale);
    sprite.setPosition(sprite.getPosition().x * screen_scale, sprite.getPosition().y * screen_scale);

    sf::RenderTexture rt;
    rt.create(screen_dim.x, screen_dim.y);
    rt.clear(fill_color);
    rt.setSmooth(true);
    rt.draw(sprite);
    rt.display();
    return write_image(imgname, rt.getTexture());
}

bool ImageManipulation::write_image(std::string imgname, const sf::Texture &t) {
    sf::Image img = t.copyToImage();
    //bug in sfml 2.3.1, will be fixed in 2.3.2
    //sfml 2.3.1 doesn't support saving images with extension "jpeg"
    const size_t dot_loc = imgname.rfind('.');
    if (dot_loc < imgname.size()) {
        const std::string extension = imgname.substr(dot_loc + 1);
        if (extension == "jpeg") {
            remove(imgname.c_str());
            imgname = imgname.substr(0, dot_loc + 1) + "jpg";
        }
    } 
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