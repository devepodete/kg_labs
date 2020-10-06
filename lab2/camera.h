//
// Created by zero on 03/10/2020.
//

#pragma once

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>

#include <iostream>
#include <cmath>

#include "keyLogger.h"

class Camera {
public:
    Camera() {
        pos = glm::vec3(0.0f, 0.0f, 4.0f);
        center = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);

        std::cout << "### Camera initialized ###" << std::endl;
    }

    glm::mat4 rotate() {
        if (Key::wPressed) {
            xRotate += moveSpeed;
        }
        if (Key::sPressed) {
            xRotate -= moveSpeed;
        }
        if (Key::aPressed) {
            yRotate -= moveSpeed;
        }
        if (Key::dPressed) {
            yRotate += moveSpeed;
        }
        if (Key::upPressed) {
            zRotate += moveSpeed;
        }
        if (Key::downPressed) {
            zRotate -= moveSpeed;
        }

        float xAbs = std::abs(xRotate);
        float yAbs = std::abs(yRotate);
        float zAbs = std::abs(zRotate);
        if (xAbs >= 4 && xAbs - 4 < 1e-2) {
            xRotate = 0.0f;
        }
        if (yAbs >= 4 && yAbs - 4 < 1e-2) {
            yRotate = 0.0f;
        }
        if (zAbs >= 4 && zAbs < 4 < 1e-2) {
            zRotate = 0.0f;
        }

        return glm::rotate(glm::mat4(1.0f), xRotate * glm::radians(90.0f),
                           glm::vec3(1.0f, 0.0f, 0.0f))
                           *
                           glm::rotate(glm::mat4(1.0f), yRotate * glm::radians(90.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f))
                           *
                           glm::rotate(glm::mat4(1.0f), zRotate * glm::radians(90.0f),
                                       glm::vec3(0.0f, 0.0f, 1.0f));

    }

    void changePos() {
        if (Key::scrollPoints.first.try_lock()) {
            pos.z += 50*moveSpeed*Key::scrollPoints.second;
            Key::scrollPoints.second = 0;
            Key::scrollPoints.first.unlock();
        }
    }

    const float moveSpeed = 0.001f;
    const float viewSpeed = 0.005f;
    float xRotate = 0.0f;
    float yRotate = 0.0f;
    float zRotate = 0.0f;

    glm::vec3 pos;
    glm::vec3 center;
    glm::vec3 up;
};
