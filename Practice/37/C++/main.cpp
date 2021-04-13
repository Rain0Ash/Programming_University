#include <iostream>
#include "point.h"
#include <cmath>
using namespace std;

double sqr(double a);
bool equal(double a, double b, double e = 1E-10);

class Vector
{
private:
    Point _point;

public:
    Vector() : _point(1, 0)
    {
    }

    Vector(const Point& end)
        : _point(end.get_x(), end.get_y())
    {
    }

    Vector(const Point& begin, const Point& end)
        : _point(end.get_x() - begin.get_x(), end.get_y() - begin.get_y())
    {
    }

    bool operator ==(const Vector& other) const
    {
        return _point == other._point;
    }

    Vector operator -() const
    {
        Point p = _point;
        p.set_x(-p.get_x());
        p.set_y(-p.get_y());

        return Vector(p);
    }

    Vector operator -(const Vector& other) const
    {
        Point p;
        p.set_x(_point.get_x() - other._point.get_x());
        p.set_y(_point.get_y() - other._point.get_y());

        return Vector(p);
    }

    Vector operator +(const Vector& other) const
    {
        Point p;
        p.set_x(_point.get_x() + other._point.get_x());
        p.set_y(_point.get_y() + other._point.get_y());

        return Vector(p);
    }

    Vector operator *(double mul) const
    {
        Point p = _point;
        p.set_x(p.get_x() * mul);
        p.set_y(p.get_y() * mul);
        return Vector(p);
    }

    double operator *(const Vector& other) const
    {
        return length() * other.length() * cos(_point.get_phi() - other._point.get_phi());
    }

    double length() const
    {
        return _point.get_r();
    }
};

int main()
{
    Vector a(Point(1, 2)), b(Point(-2, 0), Point(-1, 2));
    if (a == b && b == a) cout << "Equality test passed\n";
    else cout << "Equality test failed\n";

    Vector na(Point(-1, -2)), ox(Point(1, 0)), nox(Point(-1, 0)), oy(Point(0, 1)), noy(Point(0, -1));
    if (a == -na && na == -a && -ox == nox && -oy == noy) cout << "Invert test passed\n";
    else cout << "Invert test failed\n";

    if (ox + oy + oy == a && -ox == -a + oy + oy) cout << "Summation test passed\n";
    else cout << "Summation test failed\n";

    if (-ox + oy == oy - ox && -oy + ox == ox - oy) cout << "Subtraction test passed\n";
    else cout << "Subtraction test failed\n";

    if (ox * 3 == ox + ox + ox &&
        oy * 3 == oy + oy + oy &&
        ox * (-3) == -ox - ox - ox &&
        oy * (-3) == -oy - oy - oy) cout << "Multiplication by number test passed\n";
    else cout << "Multiplication by number test failed\n";

    if (equal(ox.length(), 1) &&
        equal(oy.length(), 1) &&
        equal((ox * 3 + oy * 4).length(), 5)) cout << "Length test passed\n";
    else cout << "Length test failed\n";

    if (equal(ox*ox, sqr(ox.length())) &&
        equal(oy*oy, sqr(oy.length())) &&
        equal((ox*3 + oy*4)*(ox*3 + oy*4), sqr((ox*3 + oy*4).length()))) cout << "Multiplication by Vector test passed\n";
    else cout << "Multiplication by Vector test failed\n";
}

bool equal(double a, double b, double e) {
    if (-e < a - b && a - b < e) return true;
    else return false;
}

double sqr(double a) {
    return a * a;
}
