#include <vector>
#include <string>
#include <SFML/Graphics.hpp>


#include "automate.h"

#include "sfml_extra.h"
#include "myprint.h"

const uint32_t windowWidth = 900;
const uint32_t windowHeight = 600;


const sf::Color bgColor = sf::Color(237, 225, 242);

bool drawKeyPointsLines = true;
bool increasePower = false;
bool decreasePower = false;


void checkboxCallback(checkbox_state_t state) {
    //std::cout << "FOO CALLED" << std::endl;
    drawKeyPointsLines = !drawKeyPointsLines;
}


void increasePowerCallback(button_state_t state) {
    if (state == sfe::BUTTON_PRESSED) {
        //std::cout << "increase callback" << std::endl;
        increasePower = true;
    }
}

void decreasePowerCallback(button_state_t state) {
    if (state == sfe::BUTTON_PRESSED) {
        decreasePower = true;
    }
}


int main() {
    sf::Font mainFont;
    if (!mainFont.loadFromFile("fonts/FreeMono.ttf")) {
        std::cout << "Failed to load font\n";
        return 0;
    }


    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            "Made by @devepodete");
    window.setPosition(sf::Vector2i(200, 10));


    atm::SplineAutomate automate(&window);
    automate.setCurvePrecision(50);


    automate.addCheckbox(sfe::Checkbox(&window, {20.0f, 20.0f}, checkboxCallback));


    sfe::Button button1 = sfe::Button(&window, {10.0f, 50.0f}, increasePowerCallback);
    button1.setText(mainFont, "Increase");
    sfe::Button button2 = sfe::Button(&window, {10.0f, 90.0f}, decreasePowerCallback);
    button2.setText(mainFont, "Decrease");

    automate.addButton({button1, button2});

    sf::Text powerText("", mainFont);
    powerText.setOutlineThickness(0.0f);
    powerText.setFillColor(sf::Color::Black);
    powerText.setPosition(10.0f, 130.0f);

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

            if (increasePower) {
                //std::cout << "curve power: " << automate.getCurvePower() << std::endl;
                automate.checkAndSetCurvePower(automate.getCurvePower()+1);
                increasePower = false;
            }

            if (decreasePower) {
                automate.checkAndSetCurvePower(automate.getCurvePower()-1);
                decreasePower = false;
            }

            window.draw(automate.curveVertices.data(), automate.curveVertices.size(), sf::LineStrip);

            for (const auto &keyPointsRectangle : automate.keyPointsRectangles) {
                window.draw(keyPointsRectangle);
            }

            automate.drawCheckboxes();
            automate.drawButtons();

            powerText.setString(std::to_string(automate.getCurvePower()));
            window.draw(powerText);

            window.display();
        }
    }

    return 0;
}