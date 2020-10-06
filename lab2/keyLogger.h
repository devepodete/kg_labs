//
// Created by zero on 03/10/2020.
//

#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <utility>
#include <mutex>

namespace Key {
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;

    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    std::pair<std::mutex, int> scrollPoints;
}

class KeyLogger {
public:
    KeyLogger() {
        std::cout << "### KeyLogger initialized ###" << std::endl;
    }

    static void key_callback(GLFWwindow* window, int key,
                             int scancode, int action, int mods) {
        switch (key) {
            case GLFW_KEY_W:
                if (action == GLFW_PRESS) {
                    Key::wPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::wPressed = false;
                }
                break;
            case GLFW_KEY_A:
                if (action == GLFW_PRESS) {
                    Key::aPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::aPressed = false;
                }
                break;
            case GLFW_KEY_S:
                if (action == GLFW_PRESS) {
                    Key::sPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::sPressed = false;
                }
                break;
            case GLFW_KEY_D:
                if (action == GLFW_PRESS) {
                    Key::dPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::dPressed = false;
                }
                break;
            case GLFW_KEY_UP:
                if (action == GLFW_PRESS) {
                    Key::upPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::upPressed = false;
                }
                break;
            case GLFW_KEY_DOWN:
                if (action == GLFW_PRESS) {
                    Key::downPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::downPressed = false;
                }
                break;
            case GLFW_KEY_LEFT:
                if (action == GLFW_PRESS) {
                    Key::leftPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::leftPressed = false;
                }
                break;
            case GLFW_KEY_RIGHT:
                if (action == GLFW_PRESS) {
                    Key::rightPressed = true;
                } else if (action == GLFW_RELEASE) {
                    Key::rightPressed = false;
                }
                break;
        }
    }

    static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        if (Key::scrollPoints.first.try_lock()) {
            Key::scrollPoints.second -= yoffset;
            Key::scrollPoints.first.unlock();
        }
    }
};
