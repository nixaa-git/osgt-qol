#pragma once
#include <optional>
#include <string>
#include <vector>

// Vector of bytes where std::nullopt represents a wildcard.
using BytePattern = std::vector<std::optional<uint8_t>>;

namespace pattern
{

// Parse an IDA-style byte pattern string.
std::optional<BytePattern> parseIDA(const std::string& pattern);

} // namespace pattern