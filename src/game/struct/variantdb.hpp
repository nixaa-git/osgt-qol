#pragma once
#include "variant.hpp"
#include <hash_map>

#pragma pack(push, 1)
class VariantDB
{
  public:
    virtual ~VariantDB();
    void Print()
    {
        printf("Listing VariantDB contents\n");
        printf("*********************\n");
        for (auto& [name, variant] : m_data)
        {
            std::string s = name + ": " + variant->Print();
            printf("%s\n", s.c_str());
        }
        printf("*********************\n");
    }

    Variant* GetVarIfExists(const std::string& key)
    {
        for (const auto& [name, variant] : m_data)
            if (name == key)
                return variant;

        return nullptr;
    }

    Variant* GetVarWithDefault(const std::string& key, const Variant& vDefault)
    {
        Variant* pData = GetVarIfExists(key);

        if (!pData)
        {
            pData = new Variant(vDefault);
            m_data[key] = pData;
        }

        return pData;
    }

    Variant* GetVar(const std::string& key)
    {
        Variant* pData = GetVarIfExists(key);

        if (!pData)
        {
            pData = new Variant;
            m_data[key] = pData;
        }

        return pData;
    }

  private:
    stdext::hash_map<std::string, Variant*> m_data;
    stdext::hash_map<std::string, void*> m_functionData;
    stdext::hash_map<std::string, Variant*>::iterator m_nextItor;
};

#pragma pack(pop)
