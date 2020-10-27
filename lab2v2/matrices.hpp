//
// Created by zero on 21/10/2020.
//

#pragma once

#include <iostream>
#include <array>
#include <initializer_list>
#include <cmath>

// linear algebra
namespace mm {

    /// Pi constant
    const double pi = 3.14159265358979f;

    // Comarison between double precision (x == y <-> |x-y| < comparePrecision)
    const double comparePrecision = 1e-10f;

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

    /// Converts angle in degrees to angle in radians
    ///
    /// \param degrees Angle in degrees
    /// \return Angle in radians
    double radians(double degrees);

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
    mat<T, 4> rotate(const mat<T, 4> &identityMatrix, T angle, const vec<T, 3> &R);

    /// Create perspective projection matrix
    ///
    /// \param l Left
    /// \param r Right
    /// \param b Bottom
    /// \param t Top
    /// \param n Near
    /// \param f Far
    /// \return Perspective projection matrix
    mat<double, 4> makePerspectiveMatrix(double l, double r, double b, double t, double n, double f);

    /// Create orthographic projection matrix
    ///
    /// \param l Left
    /// \param r Right
    /// \param b Bottom
    /// \param t Top
    /// \param n Near
    /// \param f Far
    /// \return Orthographic projection matrix
    mat<double, 4> ortho(double l, double r, double b, double t, double n, double f);

    /// Create perspective projection matrix
    ///
    /// \param fovY Field of view (angle in radians)
    /// \param n Near
    /// \param f Far
    /// \return Perspective projection matrix
    mat<double, 4> perspective(double fovY, double aspectRatio, double front, double back);


    /// x1*x2+y1*y2+z1+z2
    template <typename T, length_t L>
    T partDotProduct(const vec<T, L> &v1, const vec<T, L> &v2);

    /// Dot product
    template <typename T, length_t L>
    T dotProduct(const vec<T, L> &v1, const vec<T, L> &v2);

    /// Cross product
    template <typename T>
    vec<T, 3> crossProduct(const vec<T, 3> &v1, const vec<T, 3> &v2);

    /// Check if two vectors are perpendicular
    template <typename T, length_t L>
    bool isPerpendicular(const vec<T, L> &v1, const vec<T, L> &v2);

    /// Check if two double values are equal (using comparePrecision)
    bool floatEqual(double f1, double f2);


    typedef vec<double, 2> vec2;
    typedef vec<double, 3> vec3;
    typedef vec<double, 4> vec4;

    typedef mat<double, 2> mat2;
    typedef mat<double, 3> mat3;
    typedef mat<double, 4> mat4;

} // namespace mm

#include "matrices.inl"