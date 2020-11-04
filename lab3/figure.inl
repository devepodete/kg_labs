#pragma once


sf::Color operator*(float f, sf::Color c) {
    c.r *= f;
    c.g *= f;
    c.b *= f;

    return c;
}

sf::Color operator/(sf::Color c, float f) {
    c.r /= f;
    c.g /= f;
    c.b /= f;

    return c;
}


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

    [[nodiscard]] mm::vec3 asVector3() const {
        return mm::vec3(x, y, z);
    }

    [[nodiscard]] mm::vec4 asVector4() const {
        return mm::vec4(x, y, z, w);
    }

    void print(const std::string &name = "", char end = '\0') const {
        std::cout << name;
        std::cout << "(" << x << ", " << y << ", " << z << ", " << w << ")" << end;
    }

public:
    double x, y, z;
    double w = 1.0;
};

class Triangle {
public:
    Triangle() = default;

    Triangle(std::initializer_list<Point> l) {
        //std::cout << "Triangle initializer\n";
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

        //v1.print("v1");
        //v2.print("v2");
        n = mm::crossProduct(v1, v2);
        n.normalize();
    }

    void applyTransform(const mm::mat4 &transform) {
        for (auto &p : points) {
            //std::cout << "\n----\n";
            //p.print();
            p = transform * p.asVector4();
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

    void print(const std::string &name = "", char end = '\n') const {
        std::cout << name << ": ";
        for (const auto &p : points) {
            p.print();
        }
        std::cout << end;
    }

    bool outsideWindow() {
        return std::any_of(points.begin(), points.end(), [](Point p) {
            return (p.x < -1.0 || p.x > 1.0 || p.y < -1.0 || p.y > 1.0);
        });
    }

public:
    std::array<Point, 3> points;

    // normal vector
    mm::vec<double, 3> n;
};

class Figure {
public:
    Figure() : lightSrc(nullptr) {}

    explicit Figure(const std::vector<Triangle> &newTriangles) : Figure() {
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

    void setLightSrc(Figure *f) {
        lightSrc = f;
    }

    void setColor(sf::Color c) {
        color = c;
    }

    [[nodiscard]] sf::Color getColor() const {
        return color;
    }

    void setOutlineColor(sf::Color c) {
        outlineColor = c;
    }

    [[nodiscard]] sf::Color getOutlineColor() const {
        return outlineColor;
    }

    void setOutlineThickness(float f) {
        outlineThickness = f;
    }

    [[nodiscard]] float getOutlineThickness() const {
        return outlineThickness;
    }

    void draw(sf::RenderWindow *pWindow, const mm::mat4 &transform, const mm::vec3 &camera,
              Point lightPos = Point(0, 0, 0)) const {

        std::vector<Triangle> newCubeTriangles = triangles;

        for (auto &triangle: newCubeTriangles) {
            Triangle oldTriangle = triangle;

            triangle.applyTransform(transform);
            triangle.calculateNormalVector();

            if (mm::partDotProduct(camera, triangle.getNormal()) < 0.0) {
                continue;
            }

            sf::Color resColor = color;
            if (lightSrc != nullptr) {
                Point lightPoint = lightPos;
                Point trianglePoint = oldTriangle.points[0];
                mm::vec3 vectorToTriangle = (lightPoint-trianglePoint).asVector3();

//                lightPoint.print("light", '\n');
//                trianglePoint.print("triangle", '\n');
//                vectorToTriangle.print("vec to triangle");
//                triangle.getNormal().print("normal");
//                std::cout << mm::partDotProduct(vectorToTriangle, oldTriangle.getNormal()) << std::endl;
//                std::cout << std::string(10, '-') << std::endl;

                //if (mm::partDotProduct(vectorToTriangle, oldTriangle.getNormal()) > 0) {
                    float ambientStrength = 0.1f;
                    sf::Color ambient = ambientStrength * lightSrc->getColor();

                    float diff = std::max(mm::dotProduct(vectorToTriangle, oldTriangle.getNormal()), 0.0);
                    sf::Color diffuse = diff * lightSrc->getColor();

                    resColor = (ambient + diffuse) * color;
                //}
            }

            std::vector<sf::Vertex> newTriangle = triangle.toWindowCords(pWindow->getSize().x, pWindow->getSize().y);

            sf::ConvexShape cs;
            cs.setPointCount(3);

            cs.setOutlineColor(outlineColor);
            cs.setOutlineThickness(outlineThickness);
            cs.setFillColor(resColor);
            cs.setPoint(0, newTriangle[0].position);
            cs.setPoint(1, newTriangle[1].position);
            cs.setPoint(2, newTriangle[2].position);

            pWindow->draw(cs);
        }
    }

public:
    std::vector<Triangle> triangles;

private:
    Figure *lightSrc;

    sf::Color color;
    sf::Color outlineColor;
    float outlineThickness;
};