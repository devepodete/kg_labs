//
// Created by zero on 16/10/2020.
//

#pragma once


#include <algorithm>
#include <SFML/Graphics.hpp>

#include "sfml_help.h"


typedef unsigned checkbox_state_t;
typedef unsigned button_state_t;


// helpfull extra classes for sfml library
namespace sfe {

    enum{
        CHECKBOX_ACTIVE,
        CHECKBOX_INACTIVE,

        BUTTON_PRESSED,
        BUTTON_UNPRESSED,
        BUTTON_HOVERED
    };


    // checkbox button
    class Checkbox {
    public:
        explicit Checkbox(sf::RenderWindow *pWindow) {
            pRenderWindow = pWindow;
            //changeState();
        }

        Checkbox(sf::RenderWindow *pWindow, sf::Vector2f pos) : Checkbox(pWindow){
            setPos(pos);
        }

        Checkbox(sf::RenderWindow *pWindow, sf::Vector2f pos,
                 void (*newCallback)(checkbox_state_t)) : Checkbox(pWindow, pos) {

            setCallbackFunction(newCallback);
        }

        // bind function which will be called if chekbox state changes
        void setCallbackFunction(void (*newCallback)(checkbox_state_t curState)) {
            callback = newCallback;
        }

        void setPos(sf::Vector2f pos) {
            if (state == CHECKBOX_ACTIVE) {
                rectangleShape = sfh::squarePoint(pos, sfh::POS_LEFT_UP, CHECKBOX_ACTIVE_COLOR);
            } else if (state == CHECKBOX_INACTIVE) {
                rectangleShape = sfh::squarePoint(pos, sfh::POS_LEFT_UP, CHECKBOX_INACTIVE_COLOR);
            }
        }

        void update() {
            changeState();
            setColor();

            if (callback != nullptr) {
                callback(state);
            }
        }

        bool update(sf::Vector2f mousePos) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && sfh::pointInsideRectangle(mousePos, rectangleShape)) {
                update();
                return true;
            }

            return false;
        }

        void changeState() {
            switch (state) {
                case CHECKBOX_ACTIVE:
                    state = CHECKBOX_INACTIVE;
                    break;
                case CHECKBOX_INACTIVE:
                    state = CHECKBOX_ACTIVE;
                    break;
                default:
                    state = CHECKBOX_INACTIVE;
                    break;
            }
        }

        void setColor() {
            switch (state) {
                case CHECKBOX_ACTIVE:
                    rectangleShape.setFillColor(CHECKBOX_ACTIVE_COLOR);
                    break;
                case CHECKBOX_INACTIVE:
                    rectangleShape.setFillColor(CHECKBOX_INACTIVE_COLOR);
                    break;
                default:
                    rectangleShape.setFillColor(sf::Color::Black);
            }
        }

        sf::Vector2f getPos() {
            return rectangleShape.getPosition();
        }

        sf::RectangleShape getRectangle() {
            return rectangleShape;
        }

        checkbox_state_t getState() {
            return state;
        }

    public:
        sf::Color CHECKBOX_INACTIVE_COLOR = sf::Color::Red;
        sf::Color CHECKBOX_ACTIVE_COLOR = sf::Color::Green;

    private:
        sf::RenderWindow *pRenderWindow = nullptr;
        void (*callback)(checkbox_state_t) = nullptr;

        //left up corner position
        sf::RectangleShape rectangleShape;

        checkbox_state_t state = CHECKBOX_INACTIVE;
    };


    // simple button
    class Button {
    public:
        explicit Button(sf::RenderWindow *pWindow) {
            //std::cout << "Default constructor called" << std::endl;
            pRenderWindow = pWindow;
        }

        Button(sf::RenderWindow *pWindow, sf::Vector2f pos) : Button(pWindow) {
            setPos(pos);
        }

        Button(sf::RenderWindow *pWindow, sf::Vector2f pos, sf::Vector2f size) : Button(pWindow, pos) {
            setSize(size);
        }

        Button(sf::RenderWindow *pWindow, sf::Vector2f pos, void (*newCallback)(button_state_t)) :
            Button(pWindow, pos, sf::Vector2f(0, 0), newCallback) {}

        Button(sf::RenderWindow *pWindow, sf::Vector2f pos, sf::Vector2f size,
        void (*newCallback)(button_state_t)) : Button(pWindow, pos, size) {

            setCallbackFunction(newCallback);
        }


        void setText(const sf::Font &newFont, const std::string &newString, unsigned characterSize = 30,
                     float paddingX = 3.0f, float paddingY = 3.0f, float outlineThickness = 0.0f,
                     sf::Color textColor = sf::Color::Black, sf::Color outlineColor = sf::Color::Red) {

            text.setFont(newFont);
            text.setString(newString);

            text.setFillColor(textColor);
            text.setOutlineColor(outlineColor);
            text.setOutlineThickness(outlineThickness);

            float buttonX = rectangleShape.getPosition().x;
            float buttonY = rectangleShape.getPosition().y;

            //float dragUp = (float)hasCapitalLetters(newString) * (float)characterSize / 4.0f;
            float dragUp = 10.0f;
            text.setPosition(buttonX + paddingX, buttonY + paddingY - dragUp);
            text.setCharacterSize(characterSize);

            sf::Vector2f rectangleSize = {2*paddingX + (float)(newString.size()*characterSize)*0.6f,
                                          2*paddingY + (float)characterSize - dragUp};

            setSize(rectangleSize);

            //std::cout << "button pos: " << rectangleShape.getPosition() << std::endl;
            //std::cout << "button size: " << rectangleShape.getSize() << std::endl;
//            sf::Color cur = rectangleShape.getFillColor();
//            if (cur == sf::Color::White){
//                std::cout << "WHITE COLOR" << std::endl;
//            } else if (cur == BUTTON_UNPRESSED_COLOR) {
//                std::cout << "UNPRESSED COLOR" << std::endl;
//            } else {
//                std::cout << "OTHER COLOR" << std::endl;
//            }

            //std::cout << "text pos: " << text.getPosition() << std::endl;

        }

        void setPos(sf::Vector2f pos) {
            rectangleShape = sfh::squarePoint(pos, sfh::POS_LEFT_UP);
            setColor();
        }

        void setSize(sf::Vector2f size) {
            rectangleShape.setSize(size);
        }

        // bind function which will be called if chekbox state changes
        void setCallbackFunction(void (*newCallback)(button_state_t)) {
            callback = newCallback;
        }

        void changeState(button_state_t newState) {
            state = newState;
        }

        void setColor() {
            switch (state) {
                case BUTTON_PRESSED:
                    //std::cout << "BUTTON PRESSED COLOR SET" << std::endl;
                    rectangleShape.setFillColor(BUTTON_PRESSED_COLOR);
                    break;
                case BUTTON_UNPRESSED:
                    //std::cout << "BUTTON UNPRESSED COLOR SET" << std::endl;
                    rectangleShape.setFillColor(BUTTON_UNPRESSED_COLOR);
                    break;
                case BUTTON_HOVERED:
                    //std::cout << "BUTTON HOVERED COLOR SET" << std::endl;
                    rectangleShape.setFillColor(BUTTON_HOVERED_COLOR);
                    break;
                default:
                    rectangleShape.setFillColor(sf::Color::Black);
            }
        }

        void update(button_state_t newState) {
            changeState(newState);
            setColor();

            if (callback != nullptr) {
                callback(newState);
            }
        }

        bool update(sf::Vector2f mousePos) {
            if (sfh::pointInsideRectangle(mousePos, rectangleShape)) {

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    // left mouse key pressed inside some butto
                    if (state == sfe::BUTTON_PRESSED) {
                        // button is still pressing, do nothing
                        return true;
                    } else {
                        //button was unpressed or hovered, but now pressed
                        update(sfe::BUTTON_PRESSED);
                        return true;
                    }
                } else {
                    // left mouse key does not pressed, but inside some button
                    update(sfe::BUTTON_HOVERED);
                    return true;
                }

            } else {

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    // mouse is not inside current button, but left mouse clicked
                    if (state == sfe::BUTTON_PRESSED) {
                        //button is still pressed, but cursor is out of button
                        return true;
                    }
                } else {
                    if (state == sfe::BUTTON_PRESSED || state == sfe::BUTTON_HOVERED) {
                        update(sfe::BUTTON_UNPRESSED);
                        return true;
                    }
                }
            }

            return false;
        }

        static bool hasCapitalLetters(const std::string &s) {
            return std::any_of(s.begin(), s.end(), [](char c) {return c >= 'A' && c <= 'Z';});
        }

        sf::RectangleShape getRectangle() {
            return rectangleShape;
        }

        sf::Text getText() {
            return text;
        }

        button_state_t getState() const {
            return state;
        }

    public:
        sf::Color BUTTON_PRESSED_COLOR = sf::Color(187, 192, 194);
        sf::Color BUTTON_UNPRESSED_COLOR = sf::Color(236, 241, 243);
        sf::Color BUTTON_HOVERED_COLOR = sf::Color(210, 215, 217);

    private:
        sf::RenderWindow *pRenderWindow = nullptr;
        void (*callback)(button_state_t) = nullptr;

        //left up corner position
        sf::RectangleShape rectangleShape;

        sf::Text text;

        button_state_t state = BUTTON_UNPRESSED;
    };

    // window region, may be used for drawing, button placement
    class Field{

    };
} //namespace sfe