#include <iostream>
//#include <chrono>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>

#include "matrices.hpp"
#include "figure.hpp"
#include "math_help.hpp"


constexpr int width = 800;
constexpr int height = 600;


Figure customFigure(size_t precision) {
    if (precision < 2) {
        throw std::runtime_error("minimum precision is 2");
    }

    std::vector<float> radiuses = math::linspace(1.0f, 0.0f, precision);

//    std::cout << "radiuses:\n";
//    for (float f : radiuses) {
//        std::cout << f << " ";
//    }
//    std::cout << '\n';

    std::vector<std::vector<std::pair<float, float>>> circles(precision);

    //std::cout << "Circles:\n";
    for (size_t i = 0; i < precision; i++) {
        circles[i] = math::regularPolygon(precision+1, std::sqrt(radiuses[i]), 0.0, 0.0);
        circles[i].push_back(circles[i][0]);

//        for (auto &c : circles[i]) {
//            std::cout << c.first << " " << c.second << ", ";
//        }
//        std::cout << std::endl;
    }

    std::vector<Triangle> triangles;
    double z = 0.0;
    double zStep = 1.0 / circles.size();

    for (size_t i = 0; i < circles.size()-1; i++) {
        for (size_t j = 0; j < circles[i].size()-1; j++) {
            triangles.emplace_back(Triangle(
                    {circles[i][j].first, circles[i][j].second, z},
                    {circles[i][j+1].first, circles[i][j+1].second, z},
                    {circles[i+1][j].first, circles[i+1][j].second, z + zStep}
            ));

            if (i+1 == circles.size()-1) {
                continue;
            }

            triangles.emplace_back(Triangle(
                    {circles[i][j+1].first, circles[i][j+1].second, z},
                    {circles[i+1][j+1].first, circles[i+1][j+1].second, z + zStep},
                    {circles[i+1][j].first, circles[i+1][j].second, z + zStep}
            ));
        }
        z += zStep;
    }

//    for (const Triangle &t : triangles) {
//        t.print();
//    }

    return Figure(triangles);
}

Figure cubeFigure() {
    Figure f;
    std::vector<Point> p({
        Point(1.0, 1.0, -1.0), //0
        Point(-1.0, 1.0, -1.0),
        Point(-1.0, -1.0, -1.0),
        Point(1.0, -1.0, -1.0),

        Point(1.0, 1.0, 1.0), //4
        Point(-1.0, 1.0, 1.0),
        Point(-1.0, -1.0, 1.0),
        Point(1.0, -1.0, 1.0),
    });

    f.addTriangle(Triangle(p[0], p[1], p[4]));
    f.addTriangle(Triangle(p[1], p[5], p[4]));
    f.addTriangle(Triangle(p[1], p[2], p[5]));
    f.addTriangle(Triangle(p[2], p[6], p[5]));
    f.addTriangle(Triangle(p[2], p[3], p[6]));
    f.addTriangle(Triangle(p[3], p[7], p[6]));
    f.addTriangle(Triangle(p[3], p[0], p[7]));
    f.addTriangle(Triangle(p[0], p[4], p[7]));

    f.addTriangle(Triangle(p[4], p[5], p[6]));
    f.addTriangle(Triangle(p[6], p[7], p[4]));

    f.addTriangle(Triangle(p[0], p[2], p[1]));
    f.addTriangle(Triangle(p[0], p[3], p[2]));

    return f;
}

int main(){
    mm::vec3 cameraPos = {0.0, 0.0, -3.0};
    double cameraSpeed = 0.25;
    double rotateSpeed = 2.0;
    double FOV = 90;
    double rotateAngleX = 0.0;
    double rotateAngleY = 0.0;
    double rotateAngleZ = 0.0;
    size_t figurePrecision = 5;


    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(width, height), "NAKONECTO", sf::Style::Default, settings);

    sf::Color bgColor = sf::Color(20, 20, 20);

    // figure height
    double fh = 0.5;

    Figure cube = cubeFigure();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if(event.type == sf::Event::KeyPressed) {

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8)) {
                    cameraPos[1] -= cameraSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) {
                    cameraPos[1] += cameraSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) {
                    cameraPos[0] -= cameraSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)) {
                    cameraPos[0] += cameraSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0)) {
                    cameraPos[2] -= cameraSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
                    cameraPos[2] += cameraSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                    FOV += 1.0;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                    FOV -= 1.0;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                    rotateAngleX += rotateSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                    rotateAngleX -= rotateSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                    rotateAngleY += rotateSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
                    rotateAngleY -= rotateSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
                    rotateAngleZ += rotateSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
                    rotateAngleZ -= rotateSpeed;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                    figurePrecision++;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    figurePrecision--;
                }


                if (FOV > 180.0 || FOV < 0) {
                    FOV = 45.0;
                }
                //std::cout << "FOV:" << FOV << std::endl;

                if (rotateAngleX > 360.0 || rotateAngleX < -360.0) {
                    rotateAngleX = 0.0;
                }

                if (figurePrecision < 2) {
                    figurePrecision = 2;
                }
            }

            window.clear(bgColor);

            Figure myFigure = customFigure(figurePrecision);


            auto model = mm::mat4(1.0);
            auto modelCube = mm::translate(model, mm::vec3(3.0, 0.0, 0.0));
            modelCube = mm::scale(modelCube, mm::vec3(0.25, 0.25, 0.25));

            auto modelFigure = model;


            auto view = mm::mat4(1.0);
            view = mm::translate(view, cameraPos);
            view = mm::rotate(view, mm::radians(rotateAngleX), mm::vec3(1.0, 0.0, 0.0));
            view = mm::rotate(view, mm::radians(rotateAngleY), mm::vec3(0.0, 1.0, 0.0));
            view = mm::rotate(view, mm::radians(rotateAngleZ), mm::vec3(0.0, 0.0, 1.0));

            mm::mat4 projection(1.0);
            projection = mm::perspective(mm::radians(FOV), (double)width/height, 0.1, 5.0);
            //projection = mm::ortho(-3.0, 3.0, -3.0, 3.0, 0.1, 5.0);

            mm::mat4 resCube = projection*view*modelCube;
            mm::mat4 resFigure = projection*view*modelFigure;



            mm::vec3 cameraVector = cameraPos;
            cameraVector *= -1.0;
            cameraVector.normalize();

            std::vector<Triangle> newTriangles = myFigure.triangles;
            //std::cout << "Triangles: " << newTriangles.size() << std::endl;

            for (auto &triangle : newTriangles) {
                triangle.applyTransform(resFigure);
                triangle.calculateNormalVector();

                //auto dot = mm::dotProduct(cameraVector, triangle.getNormal());
                //std::cout << "dot: " << dot << std::endl;

                if (mm::dotProduct(cameraVector, triangle.getNormal()) < 0.0) {
                    continue;
                }

                std::vector<sf::Vertex> newTriangle = triangle.toWindowCords(width, height);

                sf::ConvexShape cs;
                cs.setPointCount(3);
                cs.setFillColor(sf::Color(255, 128, 60));
                cs.setOutlineColor(bgColor);
                cs.setOutlineThickness(-0.5f);

//                std::cout << "triangle: " << "(" << newTriangle[0].position.x << "," << newTriangle[0].position.y << ") "
//                    << "(" << newTriangle[1].position.x << "," << newTriangle[1].position.y << ") "
//                    << "(" << newTriangle[2].position.x << "," << newTriangle[2].position.y << ")\n";

                cs.setPoint(0, newTriangle[0].position);
                cs.setPoint(1, newTriangle[1].position);
                cs.setPoint(2, newTriangle[2].position);

                window.draw(cs);
            }


            std::vector<Triangle> newCubeTriangles = cube.triangles;
            for (auto &triangle: newCubeTriangles) {
                triangle.applyTransform(resCube);
                triangle.calculateNormalVector();

                //auto dot = mm::dotProduct(cameraVector, triangle.getNormal());
                //std::cout << "dot: " << dot << std::endl;
                //triangle.getNormal().print("normal");

                if (mm::partDotProduct(cameraVector, triangle.getNormal()) < 0.0) {
                    continue;
                }

                std::vector<sf::Vertex> newTriangle = triangle.toWindowCords(width, height);

                sf::ConvexShape cs;
                cs.setPointCount(3);
                cs.setFillColor(sf::Color(255, 255, 255));
                cs.setOutlineColor(bgColor);
                cs.setOutlineThickness(-0.5f);

                cs.setPoint(0, newTriangle[0].position);
                cs.setPoint(1, newTriangle[1].position);
                cs.setPoint(2, newTriangle[2].position);

                window.draw(cs);
            }

            //std::cout << std::string(10, '-') << std::endl;

            window.display();
        }
    }


    return 0;
}
