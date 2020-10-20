//
// Created by zero on 09/10/2020.
//

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>


template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    os << "[";
    for (size_t i = 0; i < v.size() - 1; i++) {
        os << v[i] << ", ";
    }
    os << v[v.size() - 1] << "]";
    os << std::endl;
    return os;
}

template<class T>
std::ostream &operator<<(std::ostream &os, const std::pair<T, T> &p) {
    os << "{" << p.first << ", " << p.second << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, sf::Vector2f vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const sf::RectangleShape &rec) {
    sf::Vector2f pos = rec.getPosition();
    sf::Vector2f size = rec.getSize();

    os << "rectangle cords : (" << pos.x << ", " << pos.y << ")\n";
    os << "rectangle size: [" << size.x << ", " << size.y << "]";
    return os;
}

template<class T>
void print(const std::vector<T> &v) {
    std::cout << v;
}

template<class T>
void print(const std::vector<T> &v, const std::string &name) {
    std::cout << "vector \'" << name << "\' size: " << v.size() << std::endl;
    std::cout << "vector \'" << name << "\':" << std::endl;
    print(v);
}

void annonce(const std::string &s, char c = '*') {
    std::cout << ('\n' + std::string(s.size() + 4, c) + '\n');
    std::cout << c << ' ' << s << ' ' << c;
    std::cout << ('\n' + std::string(s.size() + 4, c) + '\n');
}