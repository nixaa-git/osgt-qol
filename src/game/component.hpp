#pragma once
#include "game/variantdb.hpp"

class Entity;
struct EntityComponent
{
    uint8_t trackable_signal[32]; // 0
    std::string name;             // 72
    Entity* parent;               // 80
    VariantDB sharedDB;           // 88
};
