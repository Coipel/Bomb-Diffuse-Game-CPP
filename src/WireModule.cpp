#include <iostream>
#include <string>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "WireModule.h"

void wm::draw_instructions(sf::RenderWindow& window, std::vector<std::string> instructions) {
    sf::Font instructions_font;
    if (!instructions_font.loadFromFile("./assets/fonts/OpenSans/OpenSans-SemiBold.ttf")) 
        std::cout << "Error loading font" << std::endl;
    
    sf::Text text;
    text.setFont(instructions_font);
    text.setCharacterSize(25);
    text.setFillColor(sf::Color::Black);
    window.draw(text);

    for (int i = 0; i < int(instructions.size()); i++) {
        text.setPosition(625, 60 + 50*i);
        text.setString(instructions[i]);
        window.draw(text);
    }
}

void wm::wire_module() {
    
    const int game_width = 1200, game_height = 600;
    sf::RenderWindow window(sf::VideoMode(game_width, game_height), "Bomb-Diffuse-Game");
    sf::RectangleShape background(sf::Vector2f(game_width, game_height));
    background.setFillColor(sf::Color(192, 192, 192));

    bool left_click;
    sf::Vector2i position;

    const int wire_amount = 5;
    const int wire_width = 20, wire_height = 400;
    const int wire_start_horizontal = 100, wire_start_vertical = 100, wire_spacing_horizontal = 100;
    std::vector<bool> wire_cut_status(wire_amount, false);

    std::vector<std::string> instructions = {"---Wire-Module-Instructions---",
                                                "*Don't cut unless specified*",
                                                "---BLUE WIRES---",
                                                "~If one blue wire, cut the fourth wire~",
                                                "~If two blue wires, cut the second wire~",
                                                "---RED WIRES---",
                                                "~If four red wires or more, cut the first wire~",
                                                "---GREEN WIRES---",
                                                "~If three green wires, cut the third wire~",
                                                "~If one green wire, cut the fifth wire~"};

    

    sf::SoundBuffer cut_sfx_buffer, explosion_sfx_buffer, win_sfx_buffer;
    if (!cut_sfx_buffer.loadFromFile("./assets/sounds/gun-reload-1.wav"))
        std::cout << "Error loading buffer" << std::endl;
    if (!explosion_sfx_buffer.loadFromFile("./assets/sounds/bang_6.wav"))
        std::cout << "Error loading buffer" << std::endl;
    if (!win_sfx_buffer.loadFromFile("./assets/sounds/you_got_it_1.wav"))
        std::cout << "Error loading buffer" << std::endl;

    sf::Sound cut_sfx(cut_sfx_buffer), explosion_sfx(explosion_sfx_buffer), win_sfx(win_sfx_buffer);

    sf::Color colors[wire_amount];

    std::random_device rd;
	std::uniform_int_distribution<int> dist(0, 2);
    
    for (int i = 0; i < wire_amount; i++) {
        int random_value = dist(rd);
        if (random_value == 0)
            colors[i] = sf::Color::Blue;
        if (random_value == 1)
            colors[i] = sf::Color::Red;
        if (random_value == 2)
            colors[i] = sf::Color::Green;
    }

    int blue_count = 0, red_count = 0, green_count = 0;
    for (int i = 0; i < wire_amount; i++) {
        if (colors[i] == sf::Color::Blue)
            blue_count += 1;
        if (colors[i] == sf::Color::Red)
            red_count += 1;
        if (colors[i] == sf::Color::Green)
            green_count += 1;
    }
        
    std::vector<bool> correct_cuts(5, false);
    if (blue_count == 1)
        correct_cuts[3] = true;
    if (blue_count == 2)
        correct_cuts[1] = true;
    if (red_count == 4)
        correct_cuts[0] = true;
    if (green_count == 3)
        correct_cuts[2] = true;
    if (green_count == 1)
        correct_cuts[4] = true;

    bool failure = false;
    bool disarmed = false;

    while (window.isOpen()) {   
        window.clear();
        window.draw(background);

        wm::draw_instructions(window, instructions);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            left_click = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            position = sf::Mouse::getPosition(window);
        }

        // rectangles for ports
        sf::RectangleShape top_connector(sf::Vector2f(450, 50)), bottom_connector(sf::Vector2f(450, 50));
        top_connector.setFillColor(sf::Color(125, 125, 125));
        bottom_connector.setFillColor(sf::Color(125, 125, 125));
        top_connector.setPosition(80, 75);
        bottom_connector.setPosition(80, 475);

        sf::RectangleShape wire;
        wire.setSize(sf::Vector2f(wire_width, wire_height));

        sf::CircleShape port(15);
        port.setOrigin(port.getRadius(), port.getRadius());
        port.setFillColor(sf::Color::Black);
        
        window.draw(top_connector);
        window.draw(bottom_connector);

        // rectangles for wires and circles for ports
        for (int i = 0; i < wire_amount; i++) {
            wire.setFillColor((colors[i]));
            wire.setPosition(wire_start_horizontal + wire_spacing_horizontal*i, wire_start_vertical);
            window.draw(wire);

            port.setPosition(wire_start_horizontal + float(wire_width)/2.f +  wire_spacing_horizontal*i, wire_start_vertical);
            window.draw(port);
            port.setPosition(wire_start_horizontal + float(wire_width)/2.f +  wire_spacing_horizontal*i, wire_start_vertical + wire_height);
            window.draw(port);
        }
 
        const float cut_length = float(wire_height)*0.05;
        sf::RectangleShape cut(sf::Vector2f(wire_width, cut_length));
        cut.setFillColor(sf::Color(192, 192, 192));

        for (int i = 0; i < int(wire_cut_status.size()); i++) {
            int wire_left = wire_start_horizontal + wire_spacing_horizontal*i;
            int wire_right = wire_left + wire_width;
            
            bool within_x_range = position.x < wire_right && position.x > wire_left;
            bool within_y_range = position.y > wire_start_vertical && position.y < wire_start_horizontal + wire_height;
            
            if (within_x_range && within_y_range && left_click == true && wire_cut_status[i] == false) {
                wire_cut_status[i] = true;
                cut_sfx.play();
            }
            
            if (wire_cut_status[i] == true) {
                cut.setPosition(wire_left, wire_start_vertical + float(wire_height)/2.f - cut_length/2.f);
                window.draw(cut);
            }
        }

        // checking the cuts
        for (int i = 0; i < int(correct_cuts.size()); i++) {
            if (correct_cuts[i] == false && wire_cut_status[i] == true && failure == false) {
                failure = true;
                //std::cout << "Failure" << std::endl;
                explosion_sfx.play();
            }

            if (failure == true) {
                background.setFillColor(sf::Color::Black);
                window.draw(background);
            }

            if (correct_cuts == wire_cut_status && disarmed == false) {
                disarmed = true;
                //std::cout << "Disarmed" << std::endl;
                win_sfx.play();
            }
        }
        
        window.display();

        while (window.isOpen() && (disarmed == true || failure == true)) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }
        }
    } 
}