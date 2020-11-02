#pragma once

class Point {
public:
    Point() = default;

    Point(double x1, double y1, double z1, double w1 = 1) : x(x1), y(y1), z(z1), w(w1) {}

    Point operator-(const Point &p) const {
        return Point(x - p.x, y - p.y, z - p.z, w + p.w);
    }

    Point operator+(const Point &p) const {
        return Point(x + p.x, y + p.y, z + p.z, w + p.w);
    }

    Point operator*(const double d) const {
        return Point(d*x, d*y, d*z, d*w);
    }

    Point operator/(const double d) const {
        return (*this) * (1.0/d);
    }

    Point &operator=(const mm::vec3 &v) {
        *this = mm::vec4(v, 1.0);
        return *this;
    }

    Point &operator=(const mm::vec4 &v) {
        x = v[0];
        y = v[1];
        z = v[2];
        w = v[3];
        return *this;
    }

    Point &operator*=(const double d) {
        x *= d;
        y *= d;
        z *= d;
        w *= d;

        return *this;
    }

    Point &operator/=(const double d) {
        *this *= (1.0/d);

        return *this;
    }

    [[nodiscard]] mm::vec4 asVector() const {
        return mm::vec4(x, y, z, w);
    }

    void print(char end = '\0') const {
        std::cout << "(" << x << ", " << y << ", " << z << ", " << w << ")" << end;
    }

public:
    double x, y, z, w;
};

class Triangle {
public:
    Triangle() = default;

    Triangle(std::initializer_list<Point> l) {
        std::cout << "Triangle initializer\n";
        size_t i = 0;
        for (auto &p : l) {
            points[i++] = p;
        }
        calculateNormalVector();
    }

    Triangle(const Point &p0, const Point &p1, const Point &p2) {
        points[0] = p0;
        points[1] = p1;
        points[2] = p2;
        calculateNormalVector();
    }

    explicit Triangle(const std::array<Point, 3> &newPoints) : points(newPoints) {
        calculateNormalVector();
    }

    void setPoints(const std::array<Point, 3> &newPoints) {
        points = newPoints;
        calculateNormalVector();
    }

    void calculateNormalVector() {
        mm::vec<double, 3> v1(points[1].x - points[0].x, points[1].y - points[0].y,
                             points[1].z - points[0].z);
        mm::vec<double, 3> v2(points[2].x - points[1].x, points[2].y - points[1].y,
                             points[2].z - points[1].z);

        n = mm::crossProduct(v1, v2);
    }

    void applyTransform(const mm::mat4 &transform) {
        for (auto &p : points) {
            //std::cout << "\n----\n";
            //p.print();
            p = transform * p.asVector();
            //p.print();
            //std::cout << "\n----\n";
            p /= p.w;
            //std::cout << "\n----\n";
        }
    }


    [[nodiscard]] std::vector<sf::Vertex> toWindowCords(int width, int height) const {
        std::vector<sf::Vertex> res;
        for (auto &p : points) {
            mm::vec2 windowCords = normalizedToWindow(mm::vec2(p.x, p.y), width, height);
            res.emplace_back(sf::Vertex(sf::Vector2f(windowCords[0], windowCords[1]),
                                        sf::Color::Black));
        }
        // to close triangle
        res.push_back(res[0]);

        return res;
    }

    [[nodiscard]] mm::vec<double, 3> getNormal() const {
        return n;
    }

    void print(char end = '\n') const {
        for (const auto &p : points) {
            p.print();
        }
        std::cout << end;
    }

public:
    std::array<Point, 3> points;

    // normal vector
    mm::vec<double, 3> n;
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

    void print() const {
        std::cout << "Figure:\n";
        for (size_t i = 0; i < triangles.size(); i++) {
            std::cout << "Triangle [" << i << "]: ";
            triangles[i].print();
        }
    }

public:
    std::vector<Triangle> triangles;
};