#pragma once
#include "game/component.hpp"

class Entity
{
  public:
    EntityComponent* GetComponentByName(std::string key)
    {
        for (const auto& comp : m_components)
            if (comp->name == key)
                return comp;
        return nullptr;
    }
    Entity* GetEntityByName(std::string key)
    {
        for (const auto& ent : m_children)
            if (ent->m_name == key)
                return ent;
        return nullptr;
    }
    void PrintTreeAsText(int indent = 0)
    {
        std::string us;

        for (int i = 0; i < indent; i++)
        {
            us += "  "; // make the tree have branches more visually
        }
        us += m_name;

        if (!m_components.empty())
        {
            us += " (";
            for (auto it = m_components.begin(); it != m_components.end(); it++)
            {
                if (it != m_components.begin())
                    us += ", ";
                us += (*it)->name;
            }

            us += ")";
        }

        printf("%s\n", us.c_str());

        for (auto& ent : m_children)
        {
            ent->PrintTreeAsText(indent + 1);
        }
    }
    uint8_t trackable_signal[72];             // 0
    std::string m_name;                       // 72
    std::list<Entity*> m_children;            // 104
    std::list<EntityComponent*> m_components; // 120
    VariantDB m_sharedDB;                     // 136
    Entity* m_pParent;                        // 280
    bool m_bTaggedForDeletion;                // 288
    int m_recursiveFilterReferences;          // 292
    Variant* m_pPosVarCache;                  // 296
};