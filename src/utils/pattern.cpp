#include "pattern.hpp"
#include <sstream>
#include <string>

namespace pattern
{

std::optional<BytePattern> parseIDA(const std::string& pattern)
{
    BytePattern out;
    std::istringstream ss(pattern);
    std::string byte;
    while (ss >> byte)
    {
        if (byte == "?" || byte == "??")
            out.emplace_back(std::nullopt);
        else
        {
            try
            {
                out.emplace_back(std::stoi(byte, nullptr, 16));
            }
            catch (...)
            {
                return std::nullopt;
            }
        }
    }
    return out;
}

} // namespace pattern