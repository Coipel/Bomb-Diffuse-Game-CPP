#pragma once
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

namespace wm {
    void draw_instructions(sf::RenderWindow& window, std::vector<std::string> instructions);
    void wire_module();
}
