#pragma once
#include "game/variant.hpp"
#include <hash_map>


#pragma pack(push, 1)
typedef stdext::hash_map<std::string, Variant*> dataList;
typedef stdext::hash_map<std::string, void*> functionList;
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

    Variant* Get(const std::string& key) noexcept
    {
        for (const auto& [name, variant] : m_data)
            if (name == key)
                return variant;

        return nullptr;
    }

  private:
    dataList m_data;
    functionList m_functionData;
    dataList::iterator m_nextItor;
};

#pragma pack(pop)
