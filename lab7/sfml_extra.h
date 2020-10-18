//
// Created by zero on 16/10/2020.
//

#pragma once

#include <SFML/Graphics.hpp>
#include "sfml_help.h"


typedef unsigned checkbox_state_t;


// helpfull extra classes for sfml library
namespace sfe {

    static const sf::Color CHECKBOX_INACTIVE_COLOR = sf::Color::Red;
    static const sf::Color CHECKBOX_ACTIVE_COLOR = sf::Color::Green;

    enum{
        CHECKBOX_ACTIVE,
        CHECKBOX_INACTIVE
    };

    // checkbox button
    class Checkbox {
    public:
        explicit Checkbox(sf::RenderWindow *pWindow) {
            pRenderWindow = pWindow;
            changeState();
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
            rectangleShape = sfh::squarePoint(pos, sfh::POS_LEFT_UP);
            changeColor();
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
                    break;
            }

            changeColor();

            if (callback != nullptr) {
                callback(state);
            }
        }

        void changeColor() {
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

    private:
        sf::RenderWindow *pRenderWindow = nullptr;
        void (*callback)(checkbox_state_t curState) = nullptr;

        //left up corner position
        sf::RectangleShape rectangleShape;

        checkbox_state_t state = CHECKBOX_INACTIVE;
    };


    // simple button
    class Button {

    };

    // window region, may be used for drawing, button placement
    class Field{

    };
} //namespace sfe