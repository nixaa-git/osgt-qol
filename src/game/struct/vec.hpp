#pragma once
#include <string>

#pragma pack(push, 1)
// We don't need full-fat CL_Vec classes, use more lean structs.
class Vec2f
{
  public:
    Vec2f()
    {
        x = 0;
        y = 0;
    }
    Vec2f(float n)
    {
        x = n;
        y = n;
    }
    Vec2f(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
    float x, y;
    bool operator==(const Vec2f& rhs) const { return x == rhs.x && y == rhs.y; }
    std::string Print()
    {
        // Borrowed from Proton.
        char st[128];
        sprintf(st, "%.2f, %.2f", this->x, this->y);
        return std::string(st);
    }
};
class Vec3f
{
  public:
    Vec3f()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    Vec3f(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    float x, y, z;
    bool operator==(const Vec3f& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    std::string Print()
    {
        // Borrowed from Proton.
        char st[128];
        sprintf(st, "%.3f, %.3f, %.3f", this->x, this->y, this->z);
        return std::string(st);
    }
};
class Rectf
{
  public:
    Rectf()
    {
        left = 0;
        top = 0;
        right = 0;
        bottom = 0;
    }
    Rectf(float left, float top, float right, float bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }
    float left, top, right, bottom;
    std::string Print()
    {
        // Borrowed from Proton.
        char st[128];
        sprintf(st, "%.3f, %.3f, %.3f, %.3f", this->left, this->top, this->right, this->bottom);
        return std::string(st);
    }
};

// Map these to CL_ naming convention.
typedef Vec2f CL_Vec2f;
typedef Vec3f CL_Vec3f;
typedef Rectf CL_Rectf;
#pragma pack(pop)