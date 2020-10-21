#include <iostream>

#include <vector>
#include "matrices.h"

//#include <glm/glm.hpp>

int main(){
    mat::mat4 trans = mat::translate(mat::mat4(1.0f), mat::vec3(1.0f, 2.0f, 3.0f));
    trans.print();

    std::cout << "--------\n";
    mat::mat4 scale = mat::scale(mat::mat4(1.0f), mat::vec3(2.0f, 2.0f, 2.0f));
    scale.print();

    std::cout << "--------\n";
    mat::mat4 res = trans*scale;
    res.print();

}
