#include "patch.hpp"
#include <cstdio>
#include <format>
#include <fstream>
#include <stdexcept>

namespace patch
{

PatchManager& PatchManager::get()
{
    static PatchManager instance;
    return instance;
}

void PatchManager::applyPatchesFromFile(const std::string& fileName)
{
    // Apply integrity bypass check first.
    applyPatch("integrity_bypass");

    // First apply all core patches.
    for (const auto& [name, patch] : patchMap)
    {
        if (patch.type == PatchType::Core)
            applyPatch(name);
    }
    // Then apply user patches, according to user patch list file.
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        // If the file doesn't exist, then all user patches should be applied.
        std::fprintf(stderr, "No patch configuration file found, applying all user patches.\n");
        for (const auto& [name, patch] : patchMap)
        {
            if (patch.type == PatchType::User)
                applyPatch(name);
        }
        return;
    }
    // Otherwise, only apply the patches that are listed in the file.
    std::fprintf(stderr, "Found config file, applying patches from that.\n");
    std::string line;
    while (std::getline(file, line))
    {
        // Treat lines starting with '+' as patch names.
        if (!line.starts_with('+'))
            continue;
        line.erase(0, 1); // Remove the '+' character.
        applyPatch(line);
    }
}

void PatchManager::applyPatch(const std::string& name)
{
    if (!patchMap.contains(name))
        throw std::runtime_error(std::format("Patch {} does not exist.", name));
    if (patchMap[name].applied)
        return;
    // Try to apply the patch.
    const char* type = patchMap[name].type == PatchType::Core ? "core" : "user";
    try
    {
        patchMap[name].instance->apply();
        patchMap[name].applied = true;
        std::fprintf(stdout, "Applied %s patch %s\n", type, name.c_str());
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::format("Failed to apply patch {}: {}", name, e.what()));
    }
}

std::vector<std::string> PatchManager::getAppliedUserPatchList() const
{
    std::vector<std::string> patches;
    for (const auto& [name, patch] : patchMap)
    {
        if (patch.type == PatchType::User && patch.applied)
            patches.push_back(name);
    }
    return patches;
}

} // namespace patch