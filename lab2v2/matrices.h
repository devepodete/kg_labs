//
// Created by zero on 21/10/2020.
//

#pragma once

#include <iostream>
#include <array>
#include <initializer_list>
#include <cmath>

// linear algebra
namespace mat {

    typedef size_t length_t;
    typedef size_t pos_t;


    template<typename T, length_t L>
    class vec{
    public:
        template<typename... Types>
        explicit vec(Types... args){
            if (sizeof...(args) != L) {
                data.fill(static_cast<T>(0));
            } else {
                data = {args...};
            }
        }

        vec(const vec<T, L> &v) {
            data = v.data;
        }

        vec(const vec<T, L-1> &v, T val) {
            for (length_t i = 0; i < L-1; i++) {
                data[i] = v.data[i];
            }
            data[L-1] = val;
        }


        T &operator[](pos_t pos) {
            return data[pos];
        }

        const T &operator[](pos_t pos) const {
            return data[pos];
        }

        void print() {
            for (T f : data) {
                std::cout << f << " ";
            }
        }

    public:
        std::array<T, L> data;
    };


    template<typename T, length_t L>
    class mat{
    public:
        explicit mat() : mat(static_cast<T>(0)) {}

        explicit mat(const std::array<std::array<T, L>, L> &arr) {
            data = arr;
        }

        mat(std::initializer_list<std::initializer_list<T>> list) {
            length_t i = 0;
            length_t j = 0;

            for (auto it1 = list.begin(); it1 != list.end(); it1++) {
                for (auto it2 = it1->begin(); it2 != it1->end(); it2++) {
                    data[i][j] = *it2;
                    j++;
                }
                j = 0;
                i++;
            }
        }

        explicit mat(T diagonalElem) {
            for (length_t i = 0; i < L; i++) {
                for (length_t j = 0; j < L; j++) {
                    if (i == j) {
                        data[i][j] = diagonalElem;
                    } else {
                        data[i][j] = static_cast<T>(0);
                    }
                }
            }
        }

        mat(const mat<T, L> &other) {
            data = other.data;
        }


        std::array<T, L> &operator[](pos_t pos) {
            return data[pos];
        }

        const std::array<T, L> &operator[](pos_t pos) const {
            return data[pos];
        }

        vec<T, L> operator*(const vec<T, L> &v) const {
            vec<T, L> res;

            for (length_t i = 0; i < L; i++) {
                for (length_t j = 0; j < L; j++) {
                    res[i] += data[i][j] * v[j];
                }
            }

            return res;
        }


        mat<T, L> operator*(const mat<T, L> &m) const {
            mat<T, L> res;

            for (length_t i = 0; i < L; i++) {
                for (length_t j = 0; j < L; j++) {
                    for (length_t k = 0; k < L; k++) {
                        res[i][j] += data[i][k] * m[k][j];
                    }
                }
            }

            return res;
        }


        void print() {
            for (auto &arr : data) {
                for (T &val : arr) {
                    std::cout << val << " ";
                }
                std::cout << '\n';
            }
        }

    public:
        std::array<std::array<T, L>, L> data;
    };


    template <typename T, length_t L>
    mat<T, L> translate(const mat<T, L> &identityMatrix, const vec<T, L-1> &translationVector) {
        mat<T, L> res = identityMatrix;

        for (length_t i = 0; i < L-1; i++) {
            res[i][L-1] = translationVector[i];
        }

        return res;
    }

    template <typename T, length_t L>
    mat<T, L> scale(const mat<T, L> &identityMatrix, const vec<T, L-1> &scalingVector) {
        mat<T, L> res = identityMatrix;

        for (length_t i = 0; i < L-1; i++) {
            res[i][i] *= scalingVector[i];
        }

        return res;
    }

    template <typename T, length_t L>
    mat<T, L> rotate(const mat<T, L> &identityMatrix, float radiansAngle, const vec<T, L-1> &rotationVector) {

    }

    template <typename T, length_t L>
    mat<T, L> rotateX(const mat<T, L> &identityMatrix, float radiansAngle, const vec<T, L-1> &rotationVector) {

    }

    template <typename T, length_t L>
    mat<T, L> rotateY(const mat<T, L> &identityMatrix, float radiansAngle, const vec<T, L-1> &rotationVector) {

    }

    template <typename T, length_t L>
    mat<T, L> rotateZ(const mat<T, L> &identityMatrix, float radiansAngle, const vec<T, L-1> &rotationVector) {

    }

    float radians(float degrees) {
        // degrees * pi / 180
        return degrees * 0.017453292519943295f;
    }


    typedef vec<float, 3> vec3;
    typedef vec<float, 4> vec4;
    typedef mat<float, 3> mat3;
    typedef mat<float, 4> mat4;


} // namespace mat
