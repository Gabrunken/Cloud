#pragma once

#   

class Color
{
public:
    float r, g, b, a;

    static constexpr Color White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
    static constexpr Color Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
    static constexpr Color Transparent() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
    static constexpr Color Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
    static constexpr Color Green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
    static constexpr Color Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
    static constexpr Color Yellow() { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
    static constexpr Color Purple() { return { 1.0f, 0.0f, 1.0f, 1.0f }; }
    static constexpr Color Cyan() { return { 0.0f, 1.0f, 1.0f, 1.0f }; }
};