#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "automate.h"
#include "sfml_extra.h"
#include "myprint.h"

const uint32_t windowWidth = 900;
const uint32_t windowHeight = 600;
const sf::Vector2f windowMiddle = sf::Vector2f(static_cast<float>(windowWidth) / 2,
                                               static_cast<float>(windowHeight) / 2);

const sf::Color bgColor = sf::Color(237, 225, 242);

bool drawKeyPointsLines = true;

void checkboxCallback(checkbox_state_t state) {
    //std::cout << "FOO CALLED" << std::endl;
    drawKeyPointsLines = !drawKeyPointsLines;
}

void buttonCallback(button_state_t state) {
    switch (state) {
        case sfe::BUTTON_HOVERED:
            std::cout << "hovered" << std::endl;
            break;
        case sfe::BUTTON_PRESSED:
            std::cout << "pressed" << std::endl;
            break;
        case sfe::BUTTON_UNPRESSED:
            std::cout << "unpressed" << std::endl;
            break;
        default:
            break;
    }
}


int main() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            "Made by @devepodete");
    window.setPosition(sf::Vector2i(200, 10));

    atm::SplineAutomate automate(&window);
    //automate.setCurvePower(3);
    automate.setCurvePrecision(50);

    automate.addCheckbox(sfe::Checkbox(&window, sf::Vector2f(20.0f, 20.0f), checkboxCallback));
    automate.addButton(sfe::Button(&window, sf::Vector2f(100, 100), sf::Vector2f(50, 50), buttonCallback));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            automate.update(sf::Vector2f(sf::Mouse::getPosition(window)));

            window.clear(bgColor);

            if (drawKeyPointsLines) {
                window.draw(automate.keyPointsVertices.data(), automate.keyPointsVertices.size(), sf::LineStrip);
            }

            window.draw(automate.curveVertices.data(), automate.curveVertices.size(), sf::LineStrip);

            for (size_t i = 0; i < automate.keyPointsRectangles.size(); i++) {
                window.draw(automate.keyPointsRectangles[i]);
            }

            automate.drawCheckboxes();
            automate.drawButtons();

            window.display();
        }
    }

    return 0;
}