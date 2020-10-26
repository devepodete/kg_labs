#pragma once

class Point {
public:
    Point() = default;

    Point(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}

    Point operator-(const Point &p) const {
        return Point(x - p.x, y - p.y, z - p.z);
    }

    Point operator+(const Point &p) const {
        return Point(x + p.x, y + p.y, z + p.z);
    }

public:
    float x, y, z;
};

class Triangle {
public:
    Triangle() = default;

    explicit Triangle(const std::array<Point, 3> &newPoints) : points(newPoints) {
        calculateNormalVector();
    }

    void setPoints(const std::array<Point, 3> &newPoints) {
        points = newPoints;
        calculateNormalVector();
    }

    void calculateNormalVector() {
        mm::vec<float, 3> v1(points[1].x - points[0].x, points[1].y - points[0].y,
                             points[1].z - points[0].z);
        mm::vec<float, 3> v2(points[2].x - points[1].x, points[2].y - points[1].y,
                             points[2].z - points[1].z);

        n = mm::crossProduct(v1, v2);
    }

    [[nodiscard]] mm::vec<float, 3> getNormal() const {
        return n;
    }

public:
    std::array<Point, 3> points{};

    // normal vector
    mm::vec<float, 3> n;
};

class Figure {
public:
    Figure() = default;

    explicit Figure(const std::vector<Triangle> &newTriangles) {
        triangles = newTriangles;
    }

    void setTriangles(const std::vector<Triangle> &newTriangles) {
        triangles = newTriangles;
    }

    void addTriangle(const std::vector<Triangle> &newTriangles) {
        triangles.insert(triangles.end(), newTriangles.begin(), newTriangles.end());
    }

    void addTriangle(const Triangle &newTriangle) {
        triangles.push_back(newTriangle);
    }

    void clearTriangles() {
        triangles.clear();
    }

public:
    std::vector<Triangle> triangles;
};