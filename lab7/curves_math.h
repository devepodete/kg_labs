//
// Created by zero on 09/10/2020.
//

#pragma once

#include <vector>
#include <utility>
//#include <SFML/Graphics.hpp>

//#include "myprint.h"


std::pair<float, float> operator*(float f, std::pair<float, float> p) {
    return {f * p.first, f * p.second};
}

std::pair<float, float> operator+(std::pair<float, float> p1, std::pair<float, float> p2) {
    return {p1.first + p2.first, p1.second + p2.second};
}


namespace math {
    //return evenly spaced numbers over a specified interval
    std::vector<float> linspace(float t1, float t2, unsigned count = 100) {
        if (count == 0) {
            return {};
        }

        if (count == 1) {
            return {t1};
        }

        if (count == 2) {
            return {t1, t2};
        }

        bool reverse = false;
        if (t1 > t2) {
            std::swap(t1, t2);
            reverse = true;
        }

        std::vector<float> res(0);

        float distance = std::abs(t1 - t2);
        float step = (distance) / static_cast<float>(count - 1);

        float cur = t1;
        while (cur <= t2 || std::abs(cur - t2) < 1e-6) {
            res.push_back(cur);
            cur += step;
        }

        if (reverse) {
            std::reverse(res.begin(), res.end());
        }
        return res;
    }

}


namespace crv {
    class Bezier {
    public:
        std::vector<std::pair<float, float>> curve;

    public:
        Bezier() = default;

        Bezier(const std::vector<std::pair<float, float>> &keyPoints, unsigned pointCount = 100) {
            if (pointCount < 2) {
                throw std::runtime_error("failed to create Bezier curve with less then 2 points");
            }
            Bezier(keyPoints, math::linspace(0, 1, pointCount));
        }

        Bezier(const std::vector<std::pair<float, float>> &keyPoints, const std::vector<float> &t) {
            curve = BezierCurve(keyPoints, t);
        }

        //calculates bezier curve for two points
        void BezierTwoPoints(const std::pair<float, float> &left, const std::pair<float, float> &right,
                             std::vector<std::pair<float, float>> &dest, const std::vector<float> &t) {
            for (size_t i = 0; i < t.size(); i++) {
                dest[i] = (1 - t[i]) * left + t[i] * right;
            }
        }

        //calculates bezier curve with O(n^2*t) time and O(n*t) memory
        std::vector<std::pair<float, float>> BezierCurve(const std::vector<std::pair<float, float>> &keyPoints,
                                                         const std::vector<float> &t) {

            if (keyPoints.size() < 2 || t.size() < 2) {
                return {};
            }

            std::vector<std::vector<std::pair<float, float>>> vec(keyPoints.size() - 1,
                                                                  std::vector<std::pair<float, float>>(t.size()));
            for (size_t i = 0; i < vec.size(); i++) {
                BezierTwoPoints(keyPoints[i], keyPoints[i + 1], vec[i], t);
            }

            for (size_t last = vec.size() - 1; last > 0; last--) {
                for (size_t i = 0; i < last; i++) {
                    for (size_t j = 0; j < t.size(); j++) {
                        vec[i][j] = (1 - t[j]) * vec[i][j] + t[j] * vec[i + 1][j];
                    }
                }
            }

            return vec[0];
        }
    };
}
