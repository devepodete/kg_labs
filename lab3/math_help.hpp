//
// Created by zero on 18/10/2020.
//

#pragma once


#include <vector>
#include <cmath>
#include <algorithm>


namespace math {

    const double pi = acos(-1.0);

    /// Evenly spaced numbers over a specified interval [t1, t2]
    ///
    /// \param t1 Left border
    /// \param t2 Right border
    /// \param count Number of elements (including borders)
    /// \return vector of evenly spaced numbers
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

        float distance = t2 - t1;
        float step = distance / static_cast<float>(count - 1);

        float cur = t1;
        while (cur <= t2 || std::abs(cur - t2) < 1e-5) {
            res.push_back(cur);
            cur += step;
        }

        if (res.size() < count) {
            res.push_back(t2);
        }

        if (reverse) {
            std::reverse(res.begin(), res.end());
        }
        return res;
    }

    /// Constructs 2d regular polygon
    ///
    /// \param n Number of vertices
    /// \param r Radius
    /// \param x0, y0 Center
    /// \return Vector of regular polygon points
    std::vector<std::pair<float, float>> regularPolygon(size_t n, float r, float x0, float y0) {
        std::vector<std::pair<float, float>> res(n);

        for (size_t i = 0; i < n; i++) {
            res[i] = {x0 + r * cos(2 * pi * i / n), y0 + r * sin(2 * pi * i / n)};
        }

        return res;
    }

} //namespace math
