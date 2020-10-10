//
// Created by zero on 09/10/2020.
//

#pragma once

#include <SFML/Graphics.hpp>
#include "sfml_help.h"
#include "curves_math.h"
//#include "myprint.h"


typedef unsigned state_t;


class BezierAutomate{
public:
    BezierAutomate(sf::RenderWindow *pwindow) {
        this->pwindow = pwindow;
    }

    void updateMouse(sf::Vector2f mousePos) {
        sf::Vector2f windowBorders = sf::Vector2f(pwindow->getSize());

        if (mousePos.x < 0 || mousePos.y < 0 || mousePos.x > windowBorders.x || mousePos.y > windowBorders.y) {
            return;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            //std::cout << "Left click at " << mousePos << std::endl;

            if (state == BezierAutomate::NONE) {
                //std::cout << "--state NONE" << std::endl;
                state = BezierAutomate::ADD_POINT;

                for (size_t i = 0; i < keyPointsRectangles.size(); i++) {
                    if (sfh::pointInsideRectangle(mousePos, keyPointsRectangles[i])) {
                        //std::cout << "click inside " << i << "-th square: " << keyPointsRectangles[i] << std::endl;
                        state = BezierAutomate::MOVE_POINT;
                        
                        curSquarePos = i;
                        break;
                    }
                }

                if (state == BezierAutomate::ADD_POINT) {
                    //std::cout << "--state ADD_POINT" << std::endl;
                    keyPoints.push_back(mousePos);
                    keyPointsRectangles.push_back(sfh::squarePoint(mousePos));

                    state = BezierAutomate::NONE;

                    recreateCurve();

                    return;
                }
            }
            
            if (state == BezierAutomate::MOVE_POINT) {
                //std::cout << "--state MOVE_POINT" << std::endl;
                //std::cout << "at MOVE_POINT: " << prevMousePos << " " << mousePos << std::endl;
                if (prevMousePos != mousePos) {
                    keyPoints[curSquarePos] = mousePos;
                    keyPointsRectangles[curSquarePos].setPosition(mousePos - sfh::squarePointSize/2.0f);;
                    recreateCurve();
                }
            }

            //std::cout << std::endl;
            prevMousePos = mousePos;
        } else {
            state = BezierAutomate::NONE;
        }
    }

    void updateKeyboard() {

    }

    void recreateCurve() {
        //std::cout << "curve recreating..." << std::endl;
        std::vector<std::pair<float, float>> pairsCurve = sfh::points2pairs(keyPoints);
        bezierCurve.calculateBezierCurve(pairsCurve);
        curvePoints = sfh::pairs2points(bezierCurve.points);

        curveVertices.resize(curvePoints.size());
        for (size_t i = 0; i < curvePoints.size(); i++) {
            curveVertices[i] = sf::Vertex(curvePoints[i], curveVerticesColor);
        }

        keyPointsVertices.resize(keyPoints.size());
        for (size_t i = 0; i < keyPoints.size(); i++) {
            keyPointsVertices[i] = sf::Vertex(keyPoints[i], keyPointsVerticesColor);
        }

        //std::cout << "curve recreated" << std::endl;
    }

public:
    crv::Bezier bezierCurve;

    std::vector<sf::Vector2f> keyPoints;
    std::vector<sf::RectangleShape> keyPointsRectangles;
    std::vector<sf::Vertex> keyPointsVertices;

    std::vector<sf::Vector2f> curvePoints;
    std::vector<sf::Vertex> curveVertices;

    state_t state = BezierAutomate::NONE;

    const sf::Color keyPointsVerticesColor = sf::Color::Red;
    const sf::Color curveVerticesColor = sf::Color::Black;

    enum {
        NONE,
        ADD_POINT,
        MOVE_POINT,
    };
private:
    sf::RenderWindow *pwindow;

    sf::Vector2f prevMousePos;
    size_t curSquarePos;
};