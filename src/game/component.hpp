#pragma once
#include "game/variantdb.hpp"

class Entity;
class EntityComponent
{
  public:
    // Don't include ctor or anything yet. We don't have boost signals.

    void SetName(std::string name) { m_name = name; }
    std::string GetName() { return m_name; }

    // virtual void OnAdd(Entity* pEnt);
    // virtual void OnRemove();

    Entity* GetParent() { return m_parent; }
    VariantDB* GetShared() { return &m_sharedDB; }
    Variant* GetVar(std::string const& varName) { return m_sharedDB.GetVar(varName); }
    Variant* GetVarWithDefault(const std::string& varName, const Variant& var)
    {
        return m_sharedDB.GetVarWithDefault(varName, var);
    }

  private:
    uint8_t pad[32];      // 8
    std::string m_name;   // 32
    Entity* m_parent;     // 64
    VariantDB m_sharedDB; // 72
};
