//
// Created by zero on 09/10/2020.
//

#pragma once

#include <vector>
#include <utility>


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

} //namespace math


namespace crv {
    class Bezier {
    public:
        std::vector<std::pair<float, float>> points;

    public:
        Bezier() = default;

        Bezier(const std::vector<std::pair<float, float>> &keyPoints, unsigned pointCount = defaultPrecision) {
            calculateBezierCurve(keyPoints, pointCount);
        }

        Bezier(const std::vector<std::pair<float, float>> &keyPoints, const std::vector<float> &t) {
            calculateBezierCurve(keyPoints, t);
        }

        //calculates bezier curve for two points
        void calculateBezierTwoPoints(const std::pair<float, float> &left, const std::pair<float, float> &right,
                             std::vector<std::pair<float, float>> &dest, const std::vector<float> &t) {
            for (size_t i = 0; i < t.size(); i++) {
                dest[i] = (1 - t[i]) * left + t[i] * right;
            }
        }

        //calculates bezier curve with O(n^2*t) time and O(n*t) memory
        void calculateBezierCurve(const std::vector<std::pair<float, float>> &keyPoints,
                                                         const std::vector<float> &t) {

            if (keyPoints.size() < 2 || t.size() < 2) {
                return;
            }

            std::vector<std::vector<std::pair<float, float>>> vec(keyPoints.size() - 1,
                                                                  std::vector<std::pair<float, float>>(t.size()));
            for (size_t i = 0; i < vec.size(); i++) {
                calculateBezierTwoPoints(keyPoints[i], keyPoints[i + 1], vec[i], t);
            }

            for (size_t last = vec.size() - 1; last > 0; last--) {
                for (size_t i = 0; i < last; i++) {
                    for (size_t j = 0; j < t.size(); j++) {
                        vec[i][j] = (1 - t[j]) * vec[i][j] + t[j] * vec[i + 1][j];
                    }
                }
            }

            this->points = vec[0];
        }

        void calculateBezierCurve(const std::vector<std::pair<float, float>> &keyPoints, unsigned pointCount) {
            if (pointCount < 2) {
                throw std::runtime_error("failed to calculate Bezier curve for less then 2 points");
            }
            calculateBezierCurve(keyPoints, math::linspace(0, 1, pointCount));
        }

        void calculateBezierCurve(const std::vector<std::pair<float, float>> &keyPoints) {
            calculateBezierCurve(keyPoints, defaultPrecision);
        }
    private:
        static const unsigned defaultPrecision = 100;
    };
} //namespace crv