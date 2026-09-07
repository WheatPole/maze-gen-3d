#ifndef TRIPLET_H
#define TRIPLET_H

#include <QVector3D>
#include <QVariant>
#include <string>

template <typename T>
struct triplet {
    T x, y, z;
    triplet(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    triplet<T> operator +(QVector3D& vector) {
        if (std::is_same<T, int>::value || std::is_same<T, double>::value || std::is_same<T, float>::value)
            return triplet<T>(x + vector.x(), y + vector.y(), z + vector.z());
        else
            return *this;
    }
    triplet<T> operator +(triplet<T>& vector) {
        if (std::is_same<T, int>::value || std::is_same<T, double>::value || std::is_same<T, float>::value)
            return triplet<T>(x + vector.x, y + vector.y, z + vector.z);
        else
            return *this;
    }

    triplet<T> operator -(triplet<T>& vector) {
        if (std::is_same<T, int>::value || std::is_same<T, double>::value || std::is_same<T, float>::value)
            return triplet<T>(x - vector.x, y - vector.y, z - vector.z);
        else
            return *this;
    }
    inline void setX(T val) {
        x = val;
    }
    inline void setY(T val) {
        y = val;
    }
    inline void setZ(T val) {
        z = val;
    }
    bool operator ==(const triplet<T> &trip) const {
        return (x == trip.x) && (y == trip.y) && (z == trip.z);
    }
    bool operator <(const triplet<T> &trip) const {
        return (x < trip.x) && (y < trip.y) && (z < trip.z);
    }
    bool operator <=(const triplet<T> &trip) const {
        return (x <= trip.x) && (y <= trip.y) && (z <= trip.z);
    }
    bool operator >(const triplet<T> &trip) const {
        return (x > trip.x) && (y > trip.y) && (z > trip.z);
    }
    bool operator >=(const triplet<T> &trip) const {
        return (x >= trip.x) && (y >= trip.y) && (z >= trip.z);
    }
    inline std::string toString() const {
        return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";
    }
};

#endif // TRIPLET_H
