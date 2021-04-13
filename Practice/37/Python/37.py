from point import *
import math

def equal(a, b, e=1E-10):
    if -e < a - b < e: return True
    else: return False

class Vector:
    __point__: Point
    
    def __init__(self, begin = None, end = None):
        if begin == None:
            if end == None:
                self.__point__ = Point(1, 0)
            else:
                self.__point__ = end
        else:
            if end == None:
                self.__point__ = begin
            else:
                self.__point__ = Point(end.get_x() - begin.get_x(), end.get_y() - begin.get_y())

    
    def __eq__(self, other):
        return self.__point__ == other.__point__

    
    def __neg__(self):
        return Vector(Point(-self.__point__.get_x(), -self.__point__.get_y()))

    
    def __add__(self, other):
        p = Point()
        p.set_x(self.__point__.get_x() + other.__point__.get_x())
        p.set_y(self.__point__.get_y() + other.__point__.get_y())
        
        return Vector(p)

    
    def __sub__(self, other):
        p = Point()
        p.set_x(self.__point__.get_x() - other.__point__.get_x())
        p.set_y(self.__point__.get_y() - other.__point__.get_y())
        
        return Vector(p)

    
    def __mul__(self, other):
        if type(other) == int or type(other) == float:
            p = Point()
            p.set_x(self.__point__.get_x() * other)
            p.set_y(self.__point__.get_y() * other)
            
            return Vector(p)
        else:
            return self.length() * other.length() * math.cos(self.__point__.get_phi() - other.__point__.get_phi())

    
    def length(self):
        return self.__point__.get_r()

if __name__ == '__main__':
    a = Vector(Point(1, 2))
    b = Vector(Point(-2, 0), Point(-1, 2))
    if a == b and b == a: print('Equality test passed')
    else: print('Equality test failed')

    na  = Vector(Point(-1, -2))
    ox  = Vector(Point( 1,  0))
    nox = Vector(Point(-1,  0))
    oy  = Vector(Point( 0,  1))
    noy = Vector(Point( 0, -1))
    if a == -na and na == -a and -ox == nox and -oy == noy: print('Invert test passed')
    else: print('Invert test failed')

    if ox + oy + oy == a and -ox == -a + oy + oy: print('Summation test passed')
    else: print('Summation test failed')

    if -ox + oy == oy - ox and -oy + ox == ox - oy: print('Subtraction test passed')
    else: print('Subtraction test failed')

    if (ox * 3 == ox + ox + ox and
        oy * 3 == oy + oy + oy and
        ox * (-3) == -ox - ox - ox and
        oy * (-3) == -oy - oy - oy): print('Multiplication by number test passed')
    else: print('Multiplication by number test failed')

    if (equal(ox.length(), 1) and
        equal(oy.length(), 1) and
        equal((ox * 3 + oy * 4).length(), 5)): print('Length test passed')
    else: print('Length test failed')

    if (equal(ox*ox, ox.length()**2) and
        equal(oy*oy, oy.length()**2) and
        equal((ox*3 + oy*4)*(ox*3 + oy*4), (ox*3 + oy*4).length()**2)): print('Multiplication by Vector test passed')
    else: print('Multiplication by Vector test failed')