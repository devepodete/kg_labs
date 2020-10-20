//
// Created by zero on 18/10/2020.
//

#pragma once


#include <vector>
#include <algorithm>


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

} //namespace math
