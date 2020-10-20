//
// Created by zero on 09/10/2020.
//

#pragma once

#include <SFML/Graphics.hpp>
#include "sfml_help.h"
#include "curves_math.h"
#include "sfml_extra.h"
#include "myprint.h"


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

                    bool clickInsideSquare = false;
                    for (size_t i = 0; i < keyPointsRectangles.size(); i++) {
                        if (sfh::pointInsideRectangle(mousePos, keyPointsRectangles[i])) {
                            clickInsideSquare = true;

                            automateState = STATE_MOVE_POINT;

                            if (curActiveSquarePos.has_value()) {
                                keyPointsRectangles[curActiveSquarePos.value()].setFillColor(keyPointsDefaultColor);
                            }

                            curActiveSquarePos = i;
                            curMovingSquarePos = i;
                            break;
                        }
                    }

                    if (clickInsideSquare) {
                        keyPointsRectangles[curActiveSquarePos.value()].setFillColor(keyPointsChosenColor);
                    } else if (curActiveSquarePos.has_value()) {
                        keyPointsRectangles[curActiveSquarePos.value()].setFillColor(keyPointsDefaultColor);
                        curActiveSquarePos.reset();
                    }

                    if (automateState == STATE_ADD_POINT) {
                        keyPoints.push_back(mousePos);
                        keyPointsRectangles.push_back(sfh::squarePoint(mousePos, sfh::POS_MIDDLE,
                                                                       keyPointsDefaultColor));

                        automateState = STATE_NONE;

                        recreateCurve();

                        return;
                    }
                }

                if (automateState == STATE_MOVE_POINT) {
                    if (mousePos != prevMousePos) {
                        keyPoints[curMovingSquarePos.value()] = mousePos;
                        keyPointsRectangles[curMovingSquarePos.value()].setPosition(mousePos - sfh::SQUARE_POINT_SIZE / 2.0f);
                        keyPointsRectangles[curMovingSquarePos.value()].setFillColor(keyPointsMoveColor);

                        recreateCurve();
                    }
                }

                prevMousePos = mousePos;
            } else {
                automateState = STATE_NONE;

                if (curMovingSquarePos.has_value()) {
                    keyPointsRectangles[curMovingSquarePos.value()].setFillColor(keyPointsDefaultColor);
                }
                if (curActiveSquarePos.has_value()) {
                    keyPointsRectangles[curActiveSquarePos.value()].setFillColor(keyPointsChosenColor);
                }
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

        // colors of key points
        sf::Color keyPointsDefaultColor = sf::Color::White;
        sf::Color keyPointsMoveColor = sf::Color(180, 180, 180);
        sf::Color keyPointsChosenColor = sf::Color::Yellow;

    protected:
        sf::RenderWindow *pRenderWindow;

        sf::Vector2f prevMousePos;

        // index of currently moving square
        std::optional<size_t> curMovingSquarePos;

        // index of currently active square (which was selected by LMC)
        std::optional<size_t> curActiveSquarePos;
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

            if (keyPoints.size() < 2) {
                return;
            }

            std::vector<std::pair<float, float>> pairsCurve = sfh::points2pairs(keyPoints);

            splineCurve.setKeyPoints(pairsCurve);
            splineCurve.setPower(curvePower);
            splineCurve.setPrecision(curvePrecision);
            splineCurve.calculateCurve();

            std::vector<sf::Vector2f> curvePoints = sfh::pairs2points(splineCurve.points);
            curveVertices.resize(curvePoints.size());
            for (size_t i = 0; i < curvePoints.size(); i++) {
                curveVertices[i] = sf::Vertex(curvePoints[i], curveVerticesColor);
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
            if (!goodPower(curvePower)) {
                setCurvePower(2);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                if (goodPower(curvePower+1)) {
                    setCurvePower(curvePower+1);
                    recreateCurve();
                    //std::cout << "CURVE POWER INCREASED: " << curvePower << std::endl;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                if (goodPower(curvePower-1)) {
                    setCurvePower(curvePower-1);
                    recreateCurve();
                    //std::cout << "CURVE POWER DECREASED: " << curvePower << std::endl;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Delete)) {
                if (curActiveSquarePos.has_value()) {
                    keyPoints.erase(keyPoints.begin()+curActiveSquarePos.value());
                    keyPointsRectangles.erase(keyPointsRectangles.begin()+curActiveSquarePos.value());
                    keyPointsVertices.erase(keyPointsVertices.begin()+curActiveSquarePos.value());

                    curActiveSquarePos.reset();

                    if (keyPoints.size() >= 2) {
                        setCurvePower(keyPoints.size());
                    } else {
                        setCurvePower(2);
                        curveVertices.clear();
                    }

                    recreateCurve();
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

        void checkAndSetCurvePower(power_t newPower) {
            if (splineCurve.checkPower(newPower)) {
                setCurvePower(newPower);
                recreateCurve();
            }
        }

        power_t getCurvePower() const {
            return curvePower;
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
                pRenderWindow->draw(button.getText());
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
        void setCurvePower(power_t newPower) {
            curvePower = newPower;
        }

        bool goodPower(power_t newPower) {
            return newPower <= keyPoints.size() && newPower >= 2;
        }

        bool goodPrecision(precision_t precision) {
            return precision >= 1;
        }
    };

} //namespace atm
