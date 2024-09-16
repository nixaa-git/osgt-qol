#pragma once

#pragma pack(push, 1)
// We don't need full-fat CL_Vec classes, use more lean structs.
struct Vec2f
{
    float x, y;
    bool operator==(const Vec2f& rhs) const { return x == rhs.x && y == rhs.y; }
};
struct Vec3f
{
    float x, y, z;
    bool operator==(const Vec3f& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
};
struct Rectf
{
    float left, top, right, bottom;
};

// Map these to CL_ naming convention.
typedef Vec2f CL_Vec2f;
typedef Vec3f CL_Vec3f;
typedef Rectf CL_Rectf;
#pragma pack(pop)