#include <iostream>
#include <chrono>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>

#include "matrices.hpp"
#include "figure.hpp"
#include "conversion.hpp"


constexpr int width = 800;
constexpr int height = 600;

int main(){
    mm::vec3 cameraPos = {0.0, 0.0, -3.0};
    double cameraSpeed = 0.25;
    double FOV = 90;
    double rotateAngle = 0.0;


    sf::RenderWindow window(sf::VideoMode(width, height), "NAKONECTO");
    sf::Color bgColor = sf::Color::White;

    // figure height
    double fh = 0.5;
    Figure myFigure({
        Triangle({0.0, 0.0, fh}, {1.0, 0.0, fh}, {0.7071, 0.7071, fh}),
        Triangle({0.0, 0.0, fh}, {0.7071, 0.7071, fh}, {0.0, 1.0, fh}),
        Triangle({0.0, 0.0, fh}, {0.0, 1.0, fh}, {-0.7071, 0.7071, fh}),
        Triangle({0.0, 0.0, fh}, {-0.7071, 0.7071, fh}, {-1.0, 0.0, fh}),
        Triangle({0.0, 0.0, fh}, {-1.0, 0.0, fh}, {-0.7071, -0.7071, fh}),
        Triangle({0.0, 0.0, fh}, {-0.7071, -0.7071, fh}, {0.0, -1.0, fh}),
        Triangle({0.0, 0.0, fh}, {0.0, -1.0, fh}, {0.7071, -0.7071, fh}),
        Triangle({0.0, 0.0, fh}, {0.7071, -0.7071, fh}, {1.0, 0.0, fh}),

        Triangle({0.0, 0.0, -fh}, {0.7071, 0.7071, -fh}, {1.0, 0.0, -fh}),
        Triangle({0.0, 0.0, -fh}, {0.0, 1.0, -fh}, {0.7071, 0.7071, -fh}),
        Triangle({0.0, 0.0, -fh}, {-0.7071, 0.7071, -fh}, {0.0, 1.0, -fh}),
        Triangle({0.0, 0.0, -fh}, {-1.0, 0.0, -fh}, {-0.7071, 0.7071, -fh}),
        Triangle({0.0, 0.0, -fh}, {-0.7071, -0.7071, -fh}, {-1.0, 0.0, -fh}),
        Triangle({0.0, 0.0, -fh}, {0.0, -1.0, -fh}, {-0.7071, -0.7071, -fh}),
        Triangle({0.0, 0.0, -fh}, {0.7071, -0.7071, -fh}, {0.0, -1.0, -fh}),
        Triangle({0.0, 0.0, -fh}, {1.0, 0.0, -fh}, {0.7071, -0.7071, -fh}),

    });

    std::vector<std::pair<double, double>> temp = {{1.0, 0.0}, {0.7071, 0.7071}, {0.0, 1.0}, {-0.7071, 0.7071},
                                   {-1.0, 0.0}, {-0.7071, -0.7071}, {0.0, -1.0}, {0.7071, -0.7071}, {1.0, 0.0}};

    for (int i = 0; i < temp.size()-1; i++) {
        myFigure.triangles.emplace_back(Triangle({temp[i].first, temp[i].second, -fh},
                                                 {temp[i+1].first, temp[i+1].second, fh},
                                                 {temp[i].first, temp[i].second, fh}));
        myFigure.triangles.emplace_back(Triangle({temp[i].first, temp[i].second, -fh},
                                                 {temp[i+1].first, temp[i+1].second, -fh},
                                                 {temp[i+1].first, temp[i+1].second, fh}));
    }

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
                    rotateAngle += 1.0;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                    rotateAngle -= 1.0;
                }


                if (FOV > 180.0 || FOV < 0) {
                    FOV = 45.0;
                }
                std::cout << "FOV:" << FOV << std::endl;

                if (rotateAngle > 360.0 || rotateAngle < -360.0) {
                    rotateAngle = 0.0;
                }

            }


            auto model = mm::mat4(1.0);
            model = mm::rotate(model, mm::radians(rotateAngle), mm::vec3(0.0, 1.0, 0.0));

            auto view = mm::mat4(1.0);
            view = mm::translate(view, cameraPos);

            mm::mat4 projection;
            projection = mm::perspective(mm::radians(FOV), (double)width/height, 0.1, 5.0);

            mm::mat4 res = projection*view*model;


            window.clear(bgColor);

            mm::vec4 _cameraVector = {cameraPos, 1.0};
            mm::vec3 cameraVector = {_cameraVector[0], _cameraVector[1], _cameraVector[2]};
            cameraVector *= -1.0;

            std::cout << "Camera: ";
            cameraVector.print();
            std::cout << std::string(10, '-') << std::endl;

            std::vector<Triangle> newTriangles = myFigure.triangles;
            for (auto &triangle : newTriangles) {
                triangle.applyTransform(res);
                triangle.calculateNormalVector();

                std::cout << "Triangle: ";
                triangle.print();
                std::cout << "Normal: ";
                triangle.getNormal().print();

                if (mm::partDotProduct(cameraVector, triangle.getNormal()) < 0) {
                    continue;
                }

                std::vector<sf::Vertex> newTriangle = triangle.toWindowCords(width, height);

                window.draw(newTriangle.data(), newTriangle.size(), sf::LineStrip);
            }

            window.display();
        }
    }


    return 0;
}