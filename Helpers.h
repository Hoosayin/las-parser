#pragma once
#include <string>

class StringHelpers
{
private:
	StringHelpers() = default;
	virtual ~StringHelpers() = default;

public:
	static bool IsWhitespaceString(const std::string& line);
	static void TrimWhitespaces(std::string& line);

	static void ReplaceValues(
		std::string& line, 
		const std::string& oldValue, 
		const std::string& newValue);
};