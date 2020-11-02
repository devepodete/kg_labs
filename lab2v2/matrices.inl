//
// Created by zero on 21/10/2020.
//

#pragma once

namespace mm {

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

        vec(std::initializer_list<T> list) {
            length_t i = 0;
            for (auto it = list.begin(); it != list.end(); it++) {
                data[i++] = *it;
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

        vec<T, L> operator*(double d) const {
            vec<T, L> res;
            for (length_t i = 0; i < res.size(); i++) {
                res[i] = data[i] * d;
            }
            return res;
        }

        vec<T, L> &operator=(const vec<T, L> &v) {
            this->data = v.data;
            return *this;
        }

        vec<T, L> &operator*=(double d) {
            *this = *this * d;
            return *this;
        }


        T &operator[](pos_t pos) {
            return data[pos];
        }

        const T &operator[](pos_t pos) const {
            return data[pos];
        }

        [[nodiscard]] length_t size() const {
            return L;
        }

        [[nodiscard]] double magnitude() const {
            double res = 0;

            for (auto &elem : data) {
                res += elem * elem;
            }

            return std::sqrt(res);
        }

        void normalize() {
            double len = magnitude();

            if (len == 0) {
                return;
            }

            for (length_t i = 0; i < data.size(); i++) {
                data[i] = data[i] / len;
            }
        }

        void print(char end = '\n') {
            for (T f : data) {
                std::cout << f << " ";
            }
            std::cout << end;
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

        explicit mat(const vec<T, L-1> &v) : mat(static_cast<T>(1)) {
            for (length_t i = 0; i < L-1; i++) {
                data[i][i] *= v[i];
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

        mat<T, L> &operator=(const mat<T, L> &m) {
            data = m.data;
            return *this;
        }

        void operator*=(const mat<T, L> &m) {
            *this = (*this) * m;
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


    double radians(double degrees) {
        return degrees * 0.017453292519943295;
    }


    template <typename T, length_t L>
    mat<T, L> translate(const mat<T, L> &identityMatrix, const vec<T, L-1> &translationVector) {
        mat<T, L> res = identityMatrix;

        for (length_t i = 0; i < L-1; i++) {
            res[i][L-1] += translationVector[i];
        }

        return res;
    }

    template <typename T, length_t L>
    mat<T, L> scale(const mat<T, L> &identityMatrix, const vec<T, L-1> &scalingVector) {
        mat<T, L> res(scalingVector);

        return identityMatrix * res;
    }

    template <typename T>
    mat<T, 4> rotate(const mat<T, 4> &identityMatrix, T angle, const vec<T, 3> &R) {
        double cosa = std::cos(angle);
        double sina = std::sin(angle);

        double RxSin = R[0]*sina;
        double RySin = R[1]*sina;
        double RzSin = R[2]*sina;

        double Rx1Cos = R[0]*(1-cosa);
        double Ry1Cos = R[1]*(1-cosa);
        double Rz1Cos = R[2]*(1-cosa);

        mat<T, 4> ans = {
                {cosa+R[0]*Rx1Cos, R[0]*Ry1Cos-RzSin, R[0]*Rz1Cos+RySin, 0},
                {R[1]*Rx1Cos+RzSin, cosa+R[1]*Ry1Cos, R[1]*Rz1Cos-RxSin, 0},
                {R[2]*Rx1Cos-RySin, R[2]*Ry1Cos+RxSin, cosa+R[2]*Rz1Cos, 0},
                {0, 0, 0, 1}

        };

        return identityMatrix * ans;
    }


    mat<double, 4> makePerspectiveMatrix(double l, double r, double b, double t, double n, double f) {
        mat<double, 4> res = {
                {2.0*n/(r-l), 0.0, (r+l)/(r-l), 0.0},
                {0.0, 2.0*n/(t-b), (t+b)/(t-b), 0.0},
                {0.0, 0.0f, -(f+n)/(f-n), -2.0*f*n/(f-n)},
                {0.0, 0.0, -1.0, 0.0}
        };

        return res;
    }

    mat<double, 4> makeOrthoMatrix(double l, double r, double b, double t, double n, double f) {
        mat<double, 4> res = {
                {2.0/(r-l), 0.0, 0.0, -(r+l)/(r-l)},
                {0.0, 2.0/(t-b), 0.0, -(t+b)/(t-b)},
                {0.0, 0.0, -2.0/(f-n), -(f+n)/(f-n)},
                {0.0, 0.0, 0.0, 1.0}
        };

        return res;
    }

    mat<double, 4> perspective(double fovY, double aspectRatio, double front, double back) {
        double tangent = std::tan(fovY/2);
        double height = front * tangent;
        double width = height * aspectRatio;

        std::cout << "width: " << width << " height: " << height << std::endl;

        return makePerspectiveMatrix(-width, width, -height, height, front, back);
    }

    mat<double, 4> ortho(double left, double right, double bottom, double top, double front, double back) {
        return makeOrthoMatrix(left, right, bottom, top, front, back);
    }


    template <typename T, length_t L>
    T partDotProduct(const vec<T, L> &v1, const vec<T, L> &v2) {
        T res = 0;
        for (length_t i = 0; i < v1.size(); i++) {
            res += v1[i]*v2[i];
        }

        return res;
    }

    template <typename T, length_t L>
    T dotProduct(const vec<T, L> &v1, const vec<T, L> &v2) {
        return partDotProduct(v1, v2) / (v1.magnitude() * v2.magnitude());
    }


    template <typename T>
    vec<T, 3> crossProduct(const vec<T, 3> &v1, const vec<T, 3> &v2) {
        return vec<T, 3>(v1[1]*v2[2]-v2[1]*v1[2], v2[0]*v1[2]-v1[0]*v2[2],
                         v1[0]*v2[1]-v2[0]*v1[1]);
    }

    template <typename T, length_t L>
    bool isPerpendicular(const vec<T, L> &v1, const vec<T, L> &v2) {
        return floatEqual(partDotProduct(v1, v2), 0.0);
    }

    bool floatEqual(double f1, double f2) {
        if (f1 < f2) {
            return (f2-f1) < comparePrecision;
        } else {
            return (f1-f2) < comparePrecision;
        }
    }


} // namespace mm