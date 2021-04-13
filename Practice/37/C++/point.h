#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

const double PI = 3.141592653589793;

constexpr static const double precision = 1e-10;
constexpr static const double allowed_chars_to_skip = 20;

enum CoordSystem
{
    Cartesian,
    Polar
};

class Point
{
private:
    double x, y;

public:
    Point(double a1 = 0, double a2 = 0, CoordSystem coord_system = Cartesian)
    {
        if (coord_system == Cartesian)
        {
            x = a1;
            y = a2;
            return;
        }

        x = cos(a2) * a1;
        y = sin(a2) * a1;
    }

    bool operator ==(const Point& other) const
    {
        return std::abs(x - other.x) < precision && std::abs(y - other.y) < precision;
    }

    bool operator !=(const Point& other) const
    {
        return !(*this == other);
    }

    double get_x() const
    {
        return x;
    }

    void set_x(double x)
    {
        this->x = x;
    }

    double get_y() const
    {
        return y;
    }

    void set_y(double y)
    {
        this->y = y;
    }

    double get_r() const
    {
        return sqrt(x * x + y * y);
    }

    void set_r(double r)
    {
        double phi = get_phi();
        this->x = cos(phi) * r;
        this->y = sin(phi) * r;
    }

    double get_phi() const
    {
        return atan(y / x) + (x < 0 ? PI : 0);
    }

    void set_phi(double phi)
    {
        double r = get_r();
        this->x = cos(phi) * r;
        this->y = sin(phi) * r;
    }
};

std::ostream& operator <<(std::ostream& out, const Point& p)
{
    return out << '(' << p.get_x() << ',' << p.get_y() << ')';
}

std::istream& operator >>(std::istream& in, Point& p)
{
    double a;
    in.ignore(allowed_chars_to_skip, '(');
    in >> a;
    p.set_x(a);
    in.ignore(allowed_chars_to_skip, ',');
    in >> a;
    p.set_y(a);
    in.ignore(allowed_chars_to_skip, ')');
    return in;
}
