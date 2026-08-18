#ifndef TRIPLET_H
#define TRIPLET_H

#include <QVector3D>

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

};

#endif // TRIPLET_H
