//
// Created by zero on 09/10/2020.
//

#pragma once

#include <SFML/Graphics.hpp>
#include "sfml_help.h"
#include "curves_math.h"
#include "sfml_extra.h"
//#include "myprint.h"


typedef unsigned state_t;


enum automateStates {
    STATE_NONE,
    STATE_ADD_POINT,
    STATE_MOVE_POINT,
};


namespace atm {

    // abstract automate class for curve manipulations
    class Automate {
    public:
        explicit Automate(sf::RenderWindow *pWindow) {
            pRenderWindow = pWindow;
        }

        virtual void update(sf::Vector2f mousePos) {
            updateMouse(mousePos);
            updateKeyboard();
        }

        virtual void updateMouse(sf::Vector2f mousePos) {
            sf::Vector2f windowBorders = sf::Vector2f(pRenderWindow->getSize());

            if (mousePos.x < 0 || mousePos.y < 0 || mousePos.x > windowBorders.x || mousePos.y > windowBorders.y) {
                return;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                //std::cout << "CLICK AT " << mousePos.x << " " << mousePos.y << std::endl;
                if (automateState == STATE_NONE) {
                    automateState = STATE_ADD_POINT;

                    for (size_t i = 0; i < keyPointsRectangles.size(); i++) {
                        if (sfh::pointInsideRectangle(mousePos, keyPointsRectangles[i])) {
                            automateState = STATE_MOVE_POINT;

                            curSquarePos = i;
                            break;
                        }
                    }

                    if (automateState == STATE_ADD_POINT) {
                        keyPoints.push_back(mousePos);
                        keyPointsRectangles.push_back(sfh::squarePoint(mousePos));

                        automateState = STATE_NONE;

                        recreateCurve();

                        return;
                    }
                }

                if (automateState == STATE_MOVE_POINT) {
                    if (prevMousePos != mousePos) {
                        keyPoints[curSquarePos] = mousePos;
                        keyPointsRectangles[curSquarePos].setPosition(mousePos - sfh::SQUARE_POINT_SIZE / 2.0f);
                        recreateCurve();
                    }
                }

                prevMousePos = mousePos;
            } else {
                automateState = STATE_NONE;
            }
        }

        // ToDo:
        // - add delete point function
        virtual void updateKeyboard() {}

        virtual void recreateCurve() = 0;

        void setCurveColor(const sf::Color &color) {
            curveVerticesColor = color;
        }

    public:
        // current automateState of automate
        state_t automateState = STATE_NONE;

        // curve key points
        std::vector<sf::Vector2f> keyPoints;
        // curve key points (rectangles on window)
        std::vector<sf::RectangleShape> keyPointsRectangles;
        // lines between curve key points
        std::vector<sf::Vertex> keyPointsVertices;

        // curve vertices for drawing
        std::vector<sf::Vertex> curveVertices;

        // color of curve
        sf::Color curveVerticesColor = sf::Color::Black;

        // color of lines between key points
        sf::Color keyPointsVerticesColor = sf::Color::Red;
    protected:
        sf::RenderWindow *pRenderWindow;

        sf::Vector2f prevMousePos;
        size_t curSquarePos;
    };


    class BezierAutomate : public Automate {
    public:
        explicit BezierAutomate(sf::RenderWindow *pWindow) : Automate(pWindow) {}

        void recreateCurve() override {
            std::vector<std::pair<float, float>> pairsCurve = sfh::points2pairs(keyPoints);
            bezierCurve.setKeyPoints(pairsCurve);
            bezierCurve.calculateCurve();

            std::vector<sf::Vector2f> curvePoints = sfh::pairs2points(bezierCurve.points);
            curveVertices.resize(curvePoints.size());
            for (size_t i = 0; i < curvePoints.size(); i++) {
                curveVertices[i] = sf::Vertex(curvePoints[i], curveVerticesColor);
            }

            keyPointsVertices.resize(keyPoints.size());
            for (size_t i = 0; i < keyPoints.size(); i++) {
                keyPointsVertices[i] = sf::Vertex(keyPoints[i], keyPointsVerticesColor);
            }
        }

    public:
        crv::Bezier bezierCurve;
    };


    class SplineAutomate : public Automate {
    public:
        explicit SplineAutomate(sf::RenderWindow *pWindow) : Automate(pWindow) {}

        void recreateCurve() override {
            //annonce("Curve recreation");

            std::vector<std::pair<float, float>> pairsCurve = sfh::points2pairs(keyPoints);

            if (enoughPoints() && goodPower(curvePower)) {
//                std::cout << "Key Points size: " << pairsCurve.size() << std::endl;
//                std::cout << "Power: " << curvePower << std::endl;
//                std::cout << "Precision: " << curvePrecision << std::endl;

                splineCurve.setKeyPoints(pairsCurve);
                splineCurve.setPower(curvePower);
                splineCurve.setPrecision(curvePrecision);
                splineCurve.calculateCurve();

                //std::cout << "curve power: " << curvePower << std::endl;
                //std::cout << "curve precision: " << curvePrecision << std::endl;
                //print(splineCurve.points, "spline curve points");
                //print(keyPoints, "keyPoints");

                std::vector<sf::Vector2f> curvePoints = sfh::pairs2points(splineCurve.points);
                curveVertices.resize(curvePoints.size());
                for (size_t i = 0; i < curvePoints.size(); i++) {
                    curveVertices[i] = sf::Vertex(curvePoints[i], curveVerticesColor);
                }
            } else {
                //annonce("Clear!", '#');
                curveVertices.clear();
            }

            keyPointsVertices.resize(keyPoints.size());
            for (size_t i = 0; i < keyPoints.size(); i++) {
                keyPointsVertices[i] = sf::Vertex(keyPoints[i], keyPointsVerticesColor);
            }
        }

        void update(sf::Vector2f mousePos) override {
            if (updateCheckboxes(mousePos)) {
                return;
            }
            if (updateButtons(mousePos)) {
                return;
            }
            //std::cout << "No checkboxes clicked. State: " << automateState << std::endl;
            updateMouse(mousePos);
            updateKeyboard();
        }

        void updateKeyboard() override {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                if (goodPower(curvePower+1)) {
                    setCurvePower(curvePower+1);
                    recreateCurve();
                    std::cout << "CURVE POWER INCREASED: " << curvePower << std::endl;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                if (goodPower(curvePower-1)) {
                    setCurvePower(curvePower-1);
                    recreateCurve();
                    std::cout << "CURVE POWER DECREASED: " << curvePower << std::endl;
                }
            }
        }

        bool updateCheckboxes(sf::Vector2f mousePos) {
            if (automateState != STATE_NONE) {
                //automate still doing some other job with graphics
                return false;
            }

            for (auto &checkbox : checkboxes) {
                if (checkbox.update(mousePos)) {
                    return true;
                }
            }
            return false;
        }

        bool updateButtons(sf::Vector2f mousePos) {
            if (automateState != STATE_NONE) {
                //automate still doing some other job with graphics
                return false;
            }

            for (auto &button : buttons) {
                if (button.update(mousePos)) {
                    return true;
                }
            }
            return false;
        }
        
        void addCheckbox(const sfe::Checkbox &newCheckbox) {
             checkboxes.push_back(newCheckbox);
        }

        void addCheckbox(sfe::Checkbox &&newCheckbox) {
            checkboxes.push_back(newCheckbox);
        }

        void addCheckbox(const std::vector<sfe::Checkbox> &newCheckBoxes) {
            for (auto &newCheckBox : newCheckBoxes) {
                checkboxes.push_back(newCheckBox);
            }
        }

        void addCheckbox(std::vector<sfe::Checkbox> &&newCheckBoxes) {
            for (auto &newCheckBox : newCheckBoxes) {
                checkboxes.push_back(newCheckBox);
            }
        }
        
        
        void addButton(const sfe::Button &newButton) {
            buttons.push_back(newButton);
        }

        void addButton(sfe::Button &&newButton) {
            buttons.push_back(newButton);
        }

        void addButton(const std::vector<sfe::Button> &newButtons) {
            for (auto &newButton : newButtons) {
                buttons.push_back(newButton);
            }
        }

        void addButton(std::vector<sfe::Button> &&newButtons) {
            for (auto &newButton : newButtons) {
                buttons.push_back(newButton);
            }
        }


        void setCurvePower(power_t newPower) {
            curvePower = newPower;
        }

        void setCurvePrecision(precision_t newPrecision) {
            if (!goodPrecision(newPrecision)) {
                throw std::runtime_error("automate: invalid curve precision set");
            }
            curvePrecision = newPrecision;
        }


        void drawCheckboxes() {
            for (auto &checkbox : checkboxes) {
                pRenderWindow->draw(checkbox.getRectangle());
            }
        }
        
        void drawButtons() {
            for (auto &button: buttons) {
                pRenderWindow->draw(button.getRectangle());
            }
        }

    public:
        crv::BSpline splineCurve;

    private:
        unsigned curvePower;
        unsigned curvePrecision;

        std::vector<sfe::Checkbox> checkboxes;
        std::vector<sfe::Button> buttons;

    private:
        bool enoughPoints() {
            return keyPoints.size() >= 2;
        }

        bool goodPower(power_t newPower) {
            return newPower <= keyPoints.size() && newPower >= 2;
        }

        bool goodPrecision(precision_t precision) {
            return precision >= 1;
        }
    };

} //namespace atm
