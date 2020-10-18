//
// Created by zero on 16/10/2020.
//

#pragma once

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
                 void (*newCallback)(checkbox_state_t curState)) : Checkbox(pWindow, pos) {

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
        void (*callback)(checkbox_state_t curState) = nullptr;

        //left up corner position
        sf::RectangleShape rectangleShape;

        checkbox_state_t state = CHECKBOX_INACTIVE;
    };


    // simple button
    class Button {
    public:
        explicit Button(sf::RenderWindow *pWindow) {
            pRenderWindow = pWindow;
        }

        Button(sf::RenderWindow *pWindow, sf::Vector2f pos) : Button(pWindow) {
            setPos(pos);
        }

        Button(sf::RenderWindow *pWindow, sf::Vector2f pos, sf::Vector2f size) : Button(pWindow, pos) {
            setSize(size);
        }

        Button(sf::RenderWindow *pWindow, sf::Vector2f pos, sf::Vector2f size,
        void (*newCallback)(button_state_t newState)) : Button(pWindow, pos, size) {

            setCallbackFunction(newCallback);
        }

        void setPos(sf::Vector2f pos) {
            rectangleShape = sfh::squarePoint(pos, sfh::POS_LEFT_UP);
        }

        void setSize(sf::Vector2f size) {
            rectangleShape.setSize(size);
        }

        // bind function which will be called if chekbox state changes
        void setCallbackFunction(void (*newCallback)(button_state_t newState)) {
            callback = newCallback;
        }

        void changeState(button_state_t newState) {
            state = newState;
        }

        button_state_t getState() {
            return state;
        }

        void setColor() {
            switch (state) {
                case BUTTON_PRESSED:
                    rectangleShape.setFillColor(BUTTON_PRESSED_COLOR);
                    break;
                case BUTTON_UNPRESSED:
                    rectangleShape.setFillColor(BUTTON_UNPRESSED_COLOR);
                    break;
                case BUTTON_HOVERED:
                    rectangleShape.setFillColor(BUTTON_HOVERED_COLOR);
                    break;
                default:
                    rectangleShape.setFillColor(sf::Color::Black);
            }
        }

        sf::RectangleShape getRectangle() {
            return rectangleShape;
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

    public:
        sf::Color BUTTON_PRESSED_COLOR = sf::Color(187, 192, 194);
        sf::Color BUTTON_UNPRESSED_COLOR = sf::Color(236, 241, 243);
        sf::Color BUTTON_HOVERED_COLOR = sf::Color(210, 215, 217);

    private:
        sf::RenderWindow *pRenderWindow = nullptr;
        void (*callback)(button_state_t newState) = nullptr;

        //left up corner position
        sf::RectangleShape rectangleShape;

        button_state_t state = BUTTON_UNPRESSED;
    };

    // window region, may be used for drawing, button placement
    class Field{

    };
} //namespace sfe