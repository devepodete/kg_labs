#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "automate.h"

const uint32_t windowWidth = 640;
const uint32_t windowHeight = 480;
const sf::Vector2f windowMiddle = sf::Vector2f(static_cast<float>(windowWidth)/2,
                                               static_cast<float>(windowHeight)/2);

const sf::Color bgColor = sf::Color(237, 225, 242);


int main() {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            "Made by @devepodete");

    BezierAutomate automate(&window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            window.clear(bgColor);

            automate.updateMouse(sf::Vector2f(sf::Mouse::getPosition(window)));
            automate.updateKeyboard();

            window.draw(automate.curveVertices.data(), automate.curveVertices.size(), sf::LineStrip);

            window.draw(automate.keyPointsVertices.data(), automate.keyPointsVertices.size(), sf::LineStrip);

            for (size_t i = 0; i < automate.keyPointsRectangles.size(); i++) {
                window.draw(automate.keyPointsRectangles[i]);
            }

            window.display();
        }
    }

    return 0;
}