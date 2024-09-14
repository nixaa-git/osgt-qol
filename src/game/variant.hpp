#pragma once
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// TODO: Setter functions
class Variant
{
  public:
    enum eType
    {
        TYPE_UNUSED,
        TYPE_FLOAT,
        TYPE_STRING,
        TYPE_VECTOR2,
        TYPE_VECTOR3,
        TYPE_UINT32,
        TYPE_ENTITY,
        TYPE_COMPONENT,
        TYPE_RECT,
        TYPE_INT32

    };
    std::string Print()
    {
        switch (m_type)
        {
        case TYPE_FLOAT:
            return std::to_string(*((float*)m_var));
            break;
        case TYPE_STRING:
            return m_string;
            break;
        case TYPE_VECTOR2:
            return "{x: " + std::to_string(*(float*)m_var) +
                   ", y: " + std::to_string(*(float*)(m_var + 4)) + "}";
            break;
        case TYPE_VECTOR3:
            return "A vec3";
            break;
        case TYPE_UINT32:
            return std::to_string(*(int32_t*)m_var);
            break;
        case TYPE_INT32:
            return std::to_string(*(int32_t*)m_var);
            break;
        case TYPE_ENTITY:
            return "An entity";
            break;
        case TYPE_COMPONENT:
            return "A component";
            break;
        case TYPE_RECT:
            return "A rect";
            break;
        case TYPE_UNUSED:
            return "Unknown";
            break;
        }

        return "Unknown";
    }
//private:
    eType m_type;
    void* m_pVoid;
    union
    {
        uint8_t m_var[16];
        float m_as_floats[4];
        uint32_t m_as_uint32s[4];
        int32_t m_as_int32s[4];
    };
    std::string m_string;
    void* m_pSig_onChanged = 0;
};

// TODO: Expand this class further when an actual need arises.
class VariantList
{
  public:
    VariantList(){};
    Variant& Get(int parmNum) { return m_variant[parmNum]; }
    Variant m_variant[6];
};