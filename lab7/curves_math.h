//
// Created by zero on 09/10/2020.
//

#pragma once

#include <vector>
#include <utility>
#include <optional>

#include "math_help.h"

typedef int precision_t;
typedef int power_t;
typedef int interval_t;


std::pair<float, float> operator*(float f, std::pair<float, float> p) {
    return {f * p.first, f * p.second};
}

std::pair<float, float> operator+(std::pair<float, float> p1, std::pair<float, float> p2) {
    return {p1.first + p2.first, p1.second + p2.second};
}


namespace crv {

    // base curve class
    class Curve {
    public:
        Curve() = default;

        virtual void calculateCurve() = 0;

        // curve key points
        std::vector<std::pair<float, float>> keyPoints;
    public:
        // vector of points in {x, y} format
        std::vector<std::pair<float, float>> points;

        precision_t precision = defaultPrecision;
        // default value for number of curve points
        static const precision_t defaultPrecision = 100;

    public:
        virtual bool checkPrecision(precision_t newPrecision) {
            return newPrecision > 0;
        }

        virtual bool checkPoints(const std::vector<std::pair<float, float>> &toCheck) {
            return !toCheck.empty();
        }
    };


    // Bezier curve
    class Bezier : public Curve {
    public:
        Bezier() = default;

        explicit Bezier(const std::vector<std::pair<float, float>> &keyPoints,
                        precision_t precision = defaultPrecision) {

            setKeyPoints(keyPoints);
            setPrecision(precision);
        }

        void calculateCurve() override {
            calculateCurve(math::linspace(0, 1, precision));
        }

        bool setKeyPoints(const std::vector<std::pair<float, float>> &newKeyPoints) {
            if (checkPoints(newKeyPoints)) {
                this->keyPoints = newKeyPoints;
                return true;
            }
            return false;
        }

        bool setPrecision(precision_t newPrecision) {
            if (checkPrecision(newPrecision)) {
                this->precision = newPrecision;
                return true;
            }
            return false;
        }

        bool checkPrecision(precision_t newPrecision) override {
            return newPrecision >= 2;
        }

        bool checkPoints(const std::vector<std::pair<float, float>> &toCheck) override {
            return toCheck.size() >= 2;
        }

    private:
        //calculates bezier curve with O(n^2*t) time and O(n*t) memory
        void calculateCurve(const std::vector<float> &t) {

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

        //calculates bezier curve for two points
        static void calculateBezierTwoPoints(const std::pair<float, float> &left, const std::pair<float, float> &right,
                                             std::vector<std::pair<float, float>> &dest, const std::vector<float> &t) {
            for (size_t i = 0; i < t.size(); i++) {
                dest[i] = (1 - t[i]) * left + t[i] * right;
            }
        }
    };


    // B-curve based on B-spline functions
    // with open uniform knot vector (like [0, 0, 0, 1, 2, 2, 2])
    class BSpline : public Curve {
    public:
        BSpline() = default;

        BSpline(const std::vector<std::pair<float, float>> &keyPoints, power_t power,
                precision_t precision = defaultPrecision) {

            setKeyPoints(keyPoints);
            setPower(power);
            setPrecision(precision);
        }


        void calculateCurve() override {
            _calculateSplineSegments();

            points.resize((n - power + 2) * (precision - 2) + (n - power + 3));
            for (auto &point : points) {
                point = {0.0f, 0.0f};
            }

            for (size_t i = 0; i < splineSegments.size(); i++) {
                if (splineSegments[i].parts.empty()) {
                    throw std::runtime_error("empty spline segment");
                }
                std::vector<float> mergedPoints = _mergeSplineSegmentParts(splineSegments[i]);

                int tVal = t[splineSegments[i].parts[0].interval.first];
                size_t begin = (tVal == 0) ? 0 : (tVal * (precision - 1));

                for (size_t idx = 0; idx < mergedPoints.size(); idx++) {
                    points[idx + begin] = points[idx + begin] + mergedPoints[idx] * keyPoints[i];
                }
            }
        }

        bool setPower(power_t newPower) {
            if (checkPower(newPower)) {
                this->power = newPower;
                return true;
            }
            return false;
        }

        bool setPrecision(precision_t newPrecision) {
            if (checkPrecision(newPrecision)) {
                this->precision = newPrecision;
                return true;
            }
            return false;
        }

        bool setKeyPoints(const std::vector<std::pair<float, float>> &newKeyPoints) {
            if (checkPoints(newKeyPoints)) {
                this->keyPoints = newKeyPoints;
                this->n = static_cast<int>(newKeyPoints.size()) - 1;
                return true;
            }
            return false;
        }

        [[nodiscard]] power_t getPower() const {
            return power;
        }

        [[nodiscard]] precision_t getPrecision() const {
            return precision;
        }

        [[nodiscard]] bool checkPower(power_t newPower) const {
            return newPower >= 2 && newPower <= keyPoints.size();
        }

        bool checkPoints(const std::vector<std::pair<float, float>> &toCheck) override {
            return toCheck.size() >= 2;
        }

        [[nodiscard]] power_t maxAvailablePower() {
            return keyPoints.size();
        }

    private:
        // n - index of last key point (indexing begins from 0)
        // power - power of curve
        int n;
        power_t power = 2;

        // open uniform knot vector (like [0, 0, 0, 1, 2, 2, 2])
        std::vector<interval_t> t;

        //type of base spline part when adding to another spline
        enum baseSplinePartType {
            LEFT_SPLINE,
            RIGHT_SPLINE
        };

        // one part of base spline
        struct _baseSplinePart {
            std::vector<float> tempPoints;
            std::pair<interval_t, interval_t> interval;
        };

        struct _baseSpline {
            std::vector<_baseSplinePart> parts;
        };

        std::vector<_baseSpline> splineSegments;

    private:
        //calculates B-spline segments (i.e. N[i][k] for 0 <= i <= n)
        void _calculateSplineSegments() {
            _calculateOpenKnotVector();

            // N[i][k] = (t-t[i])/(t[i+k-1]-t[i]) * N[i][k-1] + (t[i+k]-t)/(t[i+k]-t[i+1]) * N[i+1][k-1]
            //
            // max first index of N[i][...] is n+k-2 (n - index of last point, indexing from 0)
            // example: if we have 8 points (indexes from 0 to 7) and power = 6 then
            // last base spline is N[7][6], the maximum first index that we will calculate is 7+6-1=12 (N[12][1]),
            // which is located on the rightest branch of the N[7][6] calculation tree
            std::vector<std::vector<_baseSpline>> N(t.size() - 1, std::vector<_baseSpline>(power));

            for (size_t i = 0; i < N.size(); i++) {
                N[i][0].parts.resize(1);
                N[i][0].parts[0].tempPoints.resize(precision);
                for (size_t j = 0; j < precision; j++) {
                    N[i][0].parts[0].tempPoints[j] = 1;
                }
                N[i][0].parts[0].interval.first = i;
                N[i][0].parts[0].interval.second = i + 1;
            }


            int border = static_cast<int>(N.size()) - 1;
            for (int curPower = 1; curPower < power; curPower++) {
                for (int i = 0; i < border; i++) {
                    N[i][curPower].parts.resize(0);
                    _addBaseSpline(N[i][curPower - 1], N[i][curPower], i, curPower + 1, LEFT_SPLINE);
                    _addBaseSpline(N[i + 1][curPower - 1], N[i][curPower], i, curPower + 1, RIGHT_SPLINE);
                }
                border--;
            }

            splineSegments.resize(keyPoints.size());
            for (size_t i = 0; i < splineSegments.size(); i++) {
                splineSegments[i] = N[i][power - 1];
            }
        }

        // add one base spline to other
        void _addBaseSpline(const _baseSpline &src, _baseSpline &dest, int i, power_t k, int flag) const {
            for (auto &srcPart : src.parts) {
                if (t[srcPart.interval.first] != t[srcPart.interval.second]) {
                    _baseSplinePart newPart;
                    newPart.interval = srcPart.interval;
                    newPart.tempPoints = _createBaseSplinePartPoints(srcPart, i, k, flag);

                    std::optional<int> pos = findInterval(dest, newPart.interval);
                    if (pos.has_value()) {
                        for (int j = 0; j < dest.parts[pos.value()].tempPoints.size(); j++) {
                            dest.parts[pos.value()].tempPoints[j] += newPart.tempPoints[j];
                        }
                    } else {
                        dest.parts.push_back(newPart);
                    }
                }
            }
        }

        // find specific interval in base spline
        static std::optional<int> findInterval(const _baseSpline &baseSpline,
                                               const std::pair<int, int> &interval) {
            for (int i = 0; i < baseSpline.parts.size(); i++) {
                if (baseSpline.parts[i].interval == interval) {
                    return i;
                }
            }
            return {};
        }

        // create left or right base spline parts
        [[nodiscard]] std::vector<float> _createBaseSplinePartPoints(const _baseSplinePart &part,
                                                                     int i, power_t k, int flag) const {
            std::vector<float> res(precision);

            auto l = static_cast<float>(t[part.interval.first]);
            auto r = static_cast<float>(t[part.interval.second]);
            std::vector<float> tempT = math::linspace(l, r, precision);

            for (size_t j = 0; j < res.size(); j++) {
                if (flag == LEFT_SPLINE) {
                    res[j] = (tempT[j] - t[i]) / static_cast<float>(t[i + k - 1] - t[i]) * part.tempPoints[j];
                } else {
                    res[j] = (t[i + k] - tempT[j]) / (float) (t[i + k] - t[i + 1]) * part.tempPoints[j];
                }
            }

            return res;
        }

        //calculates open uniform knot vector (like [0, 0, 0, 1, 2, 2, 2])
        void _calculateOpenKnotVector() {
            interval_t tMax = n - power + 2;

            t.clear();

            for (size_t i = 0; i < power; i++) {
                t.push_back(0);
            }
            for (interval_t i = 1; i < tMax; i++) {
                t.push_back(i);
            }
            for (size_t i = 0; i < power; i++) {
                t.push_back(tMax);
            }
        }

        // merge all base spline segments into one vector
        static std::vector<float> _mergeSplineSegmentParts(const _baseSpline &splineSegment) {
            std::vector<float> res;

            for (int i = 0; i < splineSegment.parts.size(); i++) {
                size_t j = (i == 0) ? 0 : 1;
                for (; j < splineSegment.parts[i].tempPoints.size(); j++) {
                    res.push_back(splineSegment.parts[i].tempPoints[j]);
                }
            }

            return res;
        }
    };

} //namespace crv