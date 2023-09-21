#include "pch.h"
#include "Helpers.h"

#include <algorithm>
#include <regex>

bool StringHelpers::IsWhitespaceString(const std::string& line)
{
    if (line.empty())
        return true;

    // Check if all characters are whitespaces
    return std::all_of(
        line.begin(), line.end(),
        [](char ch) {
            return std::isspace(static_cast<unsigned char>(ch));
        });
}

void StringHelpers::TrimWhitespaces(std::string& line)
{
    std::regex pattern("^\\s+|\\s+$");
    line = std::regex_replace(line, pattern, "");
}

void StringHelpers::ReplaceValues(
    std::string& line,
    const std::string& oldValue,
    const std::string& newValue)
{
    size_t nullPosition{ line.find(oldValue) };

    while (nullPosition != std::string::npos)
    {
        line.replace(nullPosition, oldValue.length(), newValue);

        // Find Next Position.
        nullPosition = line.find(oldValue, nullPosition + newValue.length());
    }
}
