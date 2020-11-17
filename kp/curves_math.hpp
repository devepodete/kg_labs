//
// Created by zero on 09/10/2020.
//

#pragma once

#include <vector>
#include <utility>
#include <optional>

#include <glm/glm.hpp>

#include "math_help.hpp"

typedef int precision_t;
typedef int power_t;
typedef int interval_t;


namespace crv {

    struct point3 {
        float x, y, z;

        point3 &operator+=(const point3 &p) {
            this->x += p.x;
            this->y += p.y;
            this->z += p.z;

            return *this;
        }

        [[nodiscard]] glm::vec3 asVec3() const {
            return glm::vec3(x, y, z);
        }
    };

    point3 operator*(float f, point3 p) {
        return {f * p.x, f * p.y, f * p.z};
    }

    point3 operator+(point3 p1, point3 p2) {
        return {p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
    }

    point3 operator-(point3 p1, point3 p2) {
        return {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
    }




    // base curve class
    class Curve {
    public:
        Curve() = default;

        virtual void calculateCurve() = 0;

        // curve key points
        std::vector<point3> keyPoints;
    public:
        // vector of points in {x, y, z} format
        std::vector<point3> points;

        precision_t precision = defaultPrecision;
        // default value for number of curve points
        static const precision_t defaultPrecision = 100;

    public:
        virtual bool checkPrecision(precision_t newPrecision) {
            return newPrecision > 0;
        }

        virtual bool checkPoints(const std::vector<point3> &toCheck) {
            return !toCheck.empty();
        }
    };


// Bezier curve
    class Bezier : public Curve {
    public:
        Bezier() = default;

        explicit Bezier(const std::vector<point3> &keyPoints,
                        precision_t precision = defaultPrecision) {

            setKeyPoints(keyPoints);
            setPrecision(precision);
        }

        void calculateCurve() override {
            std::vector<float> t = math::linspace(0.0f, 1.0f, precision);
            calculateCurve(t);
        }

        bool setKeyPoints(const std::vector<point3> &newKeyPoints) {
            //std::cout << "-- Setting keypoints with size " << newKeyPoints.size() << '\n';
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

        bool checkPoints(const std::vector<point3> &toCheck) override {
            return toCheck.size() >= 2;
        }

    private:
        //calculates bezier curve with O(n^2*t) time and O(n*t) memory
        void calculateCurve(const std::vector<float> &t) {
            //std::cout << "Calculating bezier\n";
            //std::cout << "keyPoints size: " << keyPoints.size() << " t size: " << t.size() << '\n';

            std::vector<std::vector<point3>> vec(keyPoints.size() - 1,
                                                                  std::vector<point3>(t.size()));
            //std::cout << "Step 0\n";
            for (size_t i = 0; i < vec.size(); i++) {
                calculateBezierTwoPoints(keyPoints[i], keyPoints[i + 1], vec[i], t);
            }

            //std::cout << "Step 1\n";



            for (size_t last = vec.size() - 1; last > 0; last--) {
                for (size_t i = 0; i < last; i++) {
                    for (size_t j = 0; j < t.size(); j++) {
                        vec[i][j] = (1 - t[j]) * vec[i][j] + t[j] * vec[i + 1][j];
                    }
                }
            }
            //std::cout << "Step 2\n";

            //std::cout << "sz:" << vec[0].size() << std::endl;
            this->points = vec[0];
            //std::cout << "Step 3\n";
        }

        //calculates bezier curve for two points
        static void calculateBezierTwoPoints(const point3 &left, const point3 &right,
                                             std::vector<point3> &dest, const std::vector<float> &t) {
            for (size_t i = 0; i < t.size(); i++) {
                dest[i] = (1 - t[i]) * left + t[i] * right;
            }
        }
    };

} //namespace crv