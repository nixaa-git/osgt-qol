#pragma once
#include "variant.hpp"
#include <hash_map>

class FunctionObject
{
  public:
    boost::signal<void(VariantList*)> sig_function;
};

class VariantDB
{
  public:
    virtual ~VariantDB() { DeleteAll(); }
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
        for (auto& var : m_functionData)
        {
            printf("FUN-%s\n", var.first.c_str());
        }
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

    void DeleteAll()
    {
        stdext::hash_map<std::string, Variant*>::iterator itor = m_data.begin();
        while (itor != m_data.end())
        {
            if (itor->second)
            {
                delete (itor->second);
                itor->second = 0;
            }
            itor++;
        }

        m_data.clear();
        { // so I can use "itor" again
            stdext::hash_map<std::string, FunctionObject*>::iterator itor = m_functionData.begin();
            while (itor != m_functionData.end())
            {
                delete (itor->second);
                itor++;
            }
        }
        m_functionData.clear();
    }

  private:
    stdext::hash_map<std::string, Variant*> m_data;
    stdext::hash_map<std::string, FunctionObject*> m_functionData;
    stdext::hash_map<std::string, Variant*>::iterator m_nextItor;
};
