#pragma once
#include "variant.hpp"
#include <hash_map>

class FunctionObject
{
  public:
    boost::signal<void(VariantList*)> sig_function;
};

#pragma pack(push, 1)

class VariantDB
{
  public:
    virtual ~VariantDB();
    void Print()
    {
        printf("Listing VariantDB contents\n");
        printf("*********************\n");
        for (auto& var : m_data)
        {
            std::string s = var.first + ": " + var.second->Print();
            printf("%s\n", s.c_str());
        }
        printf("*********************\n");
    }

    FunctionObject* GetFunction(const std::string& key)
    {
        FunctionObject* pData = GetFunctionIfExists(key);

        if (!pData)
        {
            pData = new FunctionObject;
            m_functionData[key] = pData;
        }

        return pData;
    }

    FunctionObject* GetFunctionIfExists(const std::string& key)
    {
        for (auto& var : m_functionData)
            if (var.first == key)
                return var.second;

        return nullptr;
    }

    void CallFunctionIfExists(const std::string& key, VariantList* pVList)
    {
        FunctionObject* pFunc = GetFunctionIfExists(key);
        if (pFunc)
        {
            pFunc->sig_function(pVList);
        }
    }

    Variant* GetVarIfExists(const std::string& key)
    {
        for (auto& var : m_data)
            if (var.first == key)
                return var.second;

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
    stdext::hash_map<std::string, FunctionObject*> m_functionData;
    stdext::hash_map<std::string, Variant*>::iterator m_nextItor;
};

#pragma pack(pop)
