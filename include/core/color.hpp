#pragma once

#include <stdlib.h>

class Color
{
public:
    float r, g, b, a;

    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

    static Color White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
    static Color Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
    static Color Transparent() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
    static Color Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
    static Color Green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
    static Color Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
    static Color Yellow() { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
    static Color Purple() { return { 1.0f, 0.0f, 1.0f, 1.0f }; }
    static Color Cyan() { return { 0.0f, 1.0f, 1.0f, 1.0f }; }

    static inline Color Random() { return { (rand() % 11) / 10.0f, (rand() % 11) / 10.0f, (rand() % 11) / 10.0f, 1.0f}; }
    
    Color(double r, double g, double b, double a) : r(r), g(b), b(b), a(a) {}
    Color(float r, float g, float b, float a) : r(r), g(b), b(b), a(a) {}
    
    bool operator==(const Color& other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};