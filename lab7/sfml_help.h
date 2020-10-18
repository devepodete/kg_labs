//
// Created by zero on 09/10/2020.
//

#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>

//#include "myprint.h"


// helpful functions for sfml library
namespace sfh {
    const float SQUARE_POINT_WIDTH = 10.0f;
    const float SQUARE_POINT_HEIGHT = 10.0f;
    const sf::Vector2f SQUARE_POINT_SIZE = sf::Vector2f(SQUARE_POINT_WIDTH, SQUARE_POINT_HEIGHT);

    enum{
        POS_MIDDLE,
        POS_LEFT_UP
    };

    sf::RectangleShape squarePoint(sf::Vector2f pos, int posType = POS_MIDDLE) {
        sf::RectangleShape rect;

        switch (posType) {
            case POS_MIDDLE:
                rect.setPosition(pos - sfh::SQUARE_POINT_SIZE / 2.0f);
                break;
            case POS_LEFT_UP:
                rect.setPosition(pos);
                break;
            default:
                break;
        }

        rect.setSize(sfh::SQUARE_POINT_SIZE);
        rect.setFillColor(sf::Color::White);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1.0f);

        return rect;
    }

    std::vector<sf::Vertex> points2vertices(const std::vector<sf::Vector2f> &v,
                                            sf::Color color) {
        std::vector<sf::Vertex> res(v.size());
        for (size_t i = 0; i < v.size(); i++) {
            res[i] = {v[i], color};
        }

        return res;
    }

    inline sf::Vector2f pair2Vec2f(const std::pair<float, float> &f) {
        return {f.first, f.second};
    }

    inline std::pair<float, float> Vec2f2pair(const sf::Vector2f &vec) {
        return std::make_pair(vec.x, vec.y);
    }

    std::vector<sf::Vector2f> pairs2points(const std::vector<std::pair<float, float>> &pairs) {
        std::vector<sf::Vector2f> res(pairs.size());
        for (size_t i = 0; i < pairs.size(); i++) {
            res[i] = pair2Vec2f(pairs[i]);
        }

        return res;
    }

    std::vector<std::pair<float, float>> points2pairs(const std::vector<sf::Vector2f> &points) {
        std::vector<std::pair<float, float>> res(points.size());
        for (size_t i = 0; i < points.size(); i++) {
            res[i] = Vec2f2pair(points[i]);
        }

        return res;
    }

    template<typename TPos = sf::Vector2f>
    bool pointInsideRectangle(TPos newPos, const sf::RectangleShape &rec) {
        auto pos = static_cast<sf::Vector2f>(newPos);

        sf::Vector2f recPos = rec.getPosition();
        sf::Vector2f recSize = rec.getSize();

        if (pos.x >= recPos.x && pos.y >= recPos.y &&
            pos.x <= recPos.x + recSize.x && pos.y <= recPos.y + recSize.y) {
            return true;
        } else {
            return false;
        }
    }
} //namespace sfh