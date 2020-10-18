//
// Created by zero on 09/10/2020.
//

#pragma once

#include <vector>
#include <utility>
#include <optional>

#include "myprint.h"
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

    protected:
        virtual bool checkPrecision(precision_t newPrecision) {
            if (newPrecision <= 0) {
                return false;
            }
            return true;
        }

        virtual bool checkPoints(const std::vector<std::pair<float, float>> &toCheck) {
            if (toCheck.empty()) {
                return false;
            }
            return true;
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


        void calculateCurve() override {
            calculateCurve(math::linspace(0, 1, precision));
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

        bool checkPrecision(precision_t precision) override {
            if (precision < 2) {
                return false;
            }
            return true;
        }

        bool checkPoints(const std::vector<std::pair<float, float>> &toCheck) override {
            if (toCheck.size() < 2) {
                return false;
            }
            return true;
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
//            std::cout << std::string(10, '-') + '\n';
//            std::cout << "calculating curve with " << keyPoints.size() << " key points" << std::endl;
//            std::cout << "k: " << power << std::endl;
//            std::cout << std::string(10, '-') + '\n';

            _calculateSplineSegments();
            //annonce("SPLINE SEGMENTS CALCULATED");

            // points between key points + key points
            points.resize((n - power + 2) * (precision - 2) + (n - power + 3));
            //std::cout << "points size: " << points.size() << std::endl;
            //std::cout << "n: " << n << " power: " << power << " precision: " << precision << std::endl;
            for (auto &point : points) {
                point = {0.0f, 0.0f};
            }

            for (size_t i = 0; i < splineSegments.size(); i++) {
                //std::cout << "----new spline segment: " << i << std::endl;
                if (splineSegments[i].parts.empty()) {
                    throw std::runtime_error("empty spline segment");
                }

//                std::cout << i << " splineSegment intervals : ";
//                for (auto &part : splineSegments[i].parts) {
//                    std::cout << part.interval << " ";
//                }
//                std::cout << std::endl;

                std::vector<float> mergedPoints = _mergeSplineSegmentParts(splineSegments[i]);
                //print(mergedPoints, "merged points of " + std::to_string(i));
                //std::cout << "Points merged" << std::endl;

                int tVal = t[splineSegments[i].parts[0].interval.first];
                size_t begin = (tVal == 0) ? 0 : (tVal * (precision - 1));
                //std::cout << "begin: " << begin << std::endl;
                //std::cout << "end: " << mergedPoints.size() << std::endl;

                for (size_t idx = 0; idx < mergedPoints.size(); idx++) {
                    points[idx + begin] = points[idx + begin] + mergedPoints[idx] * keyPoints[i];
                }
            }
            //print(points, "points");

            //annonce("CURVE CREATED SUCCESSFULLY");
        }

        bool setPower(power_t newPower) {
            if(checkPower(newPower)) {
                this->power = newPower;
                return true;
            }
            return false;
        }

        power_t getPower() {
            return power;
        }

        bool setPrecision(precision_t newPrecision) {
            if (checkPrecision(newPrecision)) {
                this->precision = newPrecision;
                return true;
            }
            return false;
        }

        precision_t getPrecision() {
            return precision;
        }

        bool setKeyPoints(const std::vector<std::pair<float, float>> &newKeyPoints) {
            if (checkPoints(newKeyPoints)) {
                this->keyPoints = newKeyPoints;
                this->n = static_cast<int>(newKeyPoints.size()) - 1;
                return true;
            }
            return false;
        }

        power_t maxAvailablePower() {
            return keyPoints.size();
        }

    private:
        // n - index of last point (indexing begins from 0)
        // power - power of curve
        int n;
        power_t power = 2;

        // open uniform knot vector (like [0, 0, 0, 1, 2, 2, 2])
        std::vector<interval_t> t;

    private:
        enum {
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
            //bool calculated = false;
        };

        std::vector<_baseSpline> splineSegments;

        //calculates B-spline segments (i.e. N[i][k] for 0 <= i <= n)
        void _calculateSplineSegments() {
            _calculateOpenKnotVector();

            //print(t, "open knots");
            // N[i][k] = (t-t[i])/(t[i+k-1]-t[i]) * N[i][k-1] + (t[i+k]-t)/(t[i+k]-t[i+1]) * N[i+1][k-1]
            // 1) maximum first index is n+k-2 (n - index of last point, indexing from 0)
            //    for example: if we have 8 points (indexes from 0 to 7) and power = 6 then
            //    last N is N[7][6], the maximum first index that we will calculate is 7+6-1=12 (N[12][1]),
            //    which is located on the rightest branch of the N[7][6] calculation tree
            //std::vector<std::vector<_baseSpline>> N(keyPoints.size() + power - 1, std::vector<_baseSpline>(power));
            std::vector<std::vector<_baseSpline>> N(t.size() - 1, std::vector<_baseSpline>(power));

            //std::cout << "N size: " << N.size() << std::endl;
            //std::cout << "t size: " << t.size() << std::endl;

            for (size_t i = 0; i < N.size(); i++) {
                //std::cout << "# initializing N[" << i << "][" << 0 << "]\n";
                N[i][0].parts.resize(1);
                N[i][0].parts[0].tempPoints.resize(precision);
                for (size_t j = 0; j < precision; j++) {
                    N[i][0].parts[0].tempPoints[j] = 1;
                }
                N[i][0].parts[0].interval.first = i;
                N[i][0].parts[0].interval.second = i + 1;
            }
            //std::cout << "N initialized" << std::endl;


            int border = static_cast<int>(N.size()) - 1;
            for (int curPower = 1; curPower < power; curPower++) {
                for (int i = 0; i < border; i++) {
                    //std::cout << "########\n";
                    //std::cout << "\n# calculating N[" << i << "][" << curPower << "]\n";
                    N[i][curPower].parts.resize(0);
                    _addBaseSpline(N[i][curPower - 1], N[i][curPower], i, curPower + 1, LEFT_SPLINE);
                    //std::cout << "# left spline done\n";
                    _addBaseSpline(N[i + 1][curPower - 1], N[i][curPower], i, curPower + 1, RIGHT_SPLINE);
//                    std::cout << "# right spline done\n";
//                    std::cout << "# N[" << i << "][" << curPower << "] calculated\n";
//                    std::cout << "########\n\n";
//                    std::cout << "N[" << i << "][" << curPower << "] parts:" << std::endl;
//                    for (auto &part : N[i][curPower].parts) {
//                        std::cout << "interval: " << part.interval << std::endl;
//                        std::cout << part.tempPoints << std::endl;
//                    }
//                    std::cout << std::string(10, '-') << std::endl;
                }
                border--;
            }

            splineSegments.resize(keyPoints.size());
            for (size_t i = 0; i < splineSegments.size(); i++) {
                splineSegments[i] = N[i][power - 1];
            }
        }

        void _addBaseSpline(const _baseSpline &src, _baseSpline &dest, int i, power_t k, int flag) {
//            if (flag == LEFT_SPLINE) {
//                std::cout << "left ";
//            } else {
//                std::cout << "right ";
//            }
//            std::cout << "src.parts.size() = " << src.parts.size() << std::endl;

            for (auto &srcPart : src.parts) {
                //std::cout << "srcPart interval -> " << srcPart.interval.first << " : " << srcPart.interval.second
                //          << std::endl;
                if (t[srcPart.interval.first] != t[srcPart.interval.second]) {
                    //std::cout << "t values -> " << t[srcPart.interval.first] << " : " << t[srcPart.interval.second]
                    //          << std::endl;
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
                } else {
                    //std::cout << "t has same values" << std::endl;
                }
            }
        }

        static std::optional<int> findInterval(const _baseSpline &baseSpline,
                                        const std::pair<int, int> &interval) {
            for (int i = 0; i < baseSpline.parts.size(); i++) {
                if (baseSpline.parts[i].interval == interval) {
                    return i;
                }
            }
            return {};
        }

        std::vector<float> _createBaseSplinePartPoints(const _baseSplinePart &part, int i, power_t k, int flag) {
            std::vector<float> res(precision);

            auto l = static_cast<float>(t[part.interval.first]);
            auto r = static_cast<float>(t[part.interval.second]);
            std::vector<float> tempT = math::linspace(l, r, precision);

//            std::cout << "l: " << l << ", r: " << r << std::endl;
//            print(tempT, "tempT");
//            std::cout << "i: " << i << ", k: " << k << std::endl;
//            std::cout << "_createBaseSplinePartPoints begin -> " << std::flush;
            for (size_t j = 0; j < res.size(); j++) {
                if (flag == LEFT_SPLINE) {
                    if (t[i + k - 1] == t[i]) {
                        throw std::runtime_error("got equal t at left");
                    }
                    res[j] = (tempT[j] - t[i]) / static_cast<float>(t[i + k - 1] - t[i]) * part.tempPoints[j];
                } else {
                    if (t[i + k] == t[i + 1]) {
//                        std::cout << "t[" << i + k << "] = " << t[i + k] << ", t[" << i + 1 << "] = " << t[i + 1]
//                                  << std::endl;
                        throw std::runtime_error("got equal t at right");
                    }
//                    std::cout << "t[i+k] = " << t[i+k] << ", tempT[j] = " << tempT[j] << std::endl;
//                    std::cout << "t[i+k]-TempT[j] = " << t[i + k] - tempT[j]
//                    << ", t[i + k] - t[i + 1] = " << t[i + k] - t[i + 1]
//                    << ", part.tempPoints[j] = " << part.tempPoints[j] << std::endl;
                    res[j] = (t[i + k] - tempT[j]) / (float)(t[i + k] - t[i + 1]) * part.tempPoints[j];
                }
            }
            //print(res, "res");
            //std::cout << std::endl;
            //std::cout << "end\n";

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

        static std::vector<float> _mergeSplineSegmentParts(const _baseSpline &splineSegment) {
            std::vector<float> res;

            //std::cout << "...merging " << splineSegment.parts.size() << " parts" << std::endl;
            for (int i = 0; i < splineSegment.parts.size(); i++) {
                size_t j = (i == 0) ? 0 : 1;
                for (; j < splineSegment.parts[i].tempPoints.size(); j++) {
                    res.push_back(splineSegment.parts[i].tempPoints[j]);
                }
            }

            return res;
        }

        bool checkPower(power_t newPower) {
            if (newPower < 2 || newPower > keyPoints.size()) {
                return false;
            }
            return true;
        }

        bool checkPoints(const std::vector<std::pair<float, float>> &toCheck) override {
            if (toCheck.size() < 2) {
                return false;
            }
            return true;
        }
    };

} //namespace crv