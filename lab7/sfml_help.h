//
// Created by zero on 09/10/2020.
//

#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>

//#include "myprint.h"

namespace sfh {
    const sf::Vector2f squarePointSize = sf::Vector2f(10.0f, 10.0f);

    sf::RectangleShape squarePoint(sf::Vector2f pos) {
        sf::RectangleShape rect;
        rect.setPosition(pos - sfh::squarePointSize / 2.0f);
        rect.setSize(sfh::squarePointSize);
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

    bool pointInsideRectangle(sf::Vector2f pos, const sf::RectangleShape &rec) {
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