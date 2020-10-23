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

    /// Pi constant
    const float pi = 3.14159265358979f;

    typedef size_t length_t;
    typedef size_t pos_t;

    /// Vector (constant size)
    ///
    /// \tparam T A floating-point scalar type
    /// \tparam L Size of vector
    template<typename T, length_t L>
    class vec;


    /// Square matrix (constant size)
    ///
    /// \tparam T A floating-point scalar type
    /// \tparam L Size of matrix
    template<typename T, length_t L>
    class mat;

    /// Creates a normalized vector (with magnitude equal to 1)
    ///
    /// \tparam T A floating-point scalar type
    /// \tparam L Size of vector
    /// \param v Input vector
    /// \return Normalized vector v
    template <typename T, length_t L>
    vec<T, L> normalize(const vec<T, L> &v);

    /// Converts angle in degrees to angle in radians
    ///
    /// \param degrees Angle in degrees
    /// \return Angle in radians
    float radians(float degrees);

    /// Add translation to identityMatrix
    ///
    /// \tparam T A floating-point scalar type
    /// \tparam L Size of matrix
    /// \param identityMatrix Transformation matrix, on the basis of which the new matrix will be calculated
    /// \param translationVector Vector to move to
    /// \return New transformation matrix
    template <typename T, length_t L>
    mat<T, L> translate(const mat<T, L> &identityMatrix, const vec<T, L-1> &translationVector);

    /// Add scale to identityMatrix
    ///
    /// \tparam T A floating-point scalar type
    /// \tparam L Size of matrix
    /// \param identityMatrix Transformation matrix, on the basis of which the new matrix will be calculated
    /// \param translationVector Vector to scale to
    /// \return New transformation matrix
    template <typename T, length_t L>
    mat<T, L> scale(const mat<T, L> &identityMatrix, const vec<T, L-1> &scalingVector);

    /// Add rotation to identityMatrix
    ///
    /// \tparam T A floating-point scalar type
    /// \param identityMatrix Transformation matrix, on the basis of which the new matrix will be calculated
    /// \param angle Angle in radians to turn
    /// \param R Vector to rotate around (must be normalized)
    /// \return New transformation matrix
    template <typename T>
    mat<T, 4> rotate(const mat<T, 4> &identityMatrix, float angle, const vec<T, 3> &R);


    typedef vec<float, 2> vec2;
    typedef vec<float, 3> vec3;
    typedef vec<float, 4> vec4;

    typedef mat<float, 2> mat2;
    typedef mat<float, 3> mat3;
    typedef mat<float, 4> mat4;
    
} // namespace mat

#include "matrices.inl"