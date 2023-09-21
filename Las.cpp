#include "pch.h"
#include "Las.h"
#include "Helpers.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

void Las::SetDelimitedLine(SECTION section, const DELIMITED_LINE& delimitedLine)
{
    switch (section)
    {
    case SECTION::VERSION:
        m_versionInformation.push_back(delimitedLine);
        break;
    case SECTION::WELL:
        m_wellInformation.push_back(delimitedLine);
        break;
    case SECTION::CURVE:
        m_curveInformation.push_back(delimitedLine);
        break;
    case SECTION::PARAMETER:
        m_parameterInformation.push_back(delimitedLine);
        break;
    case SECTION::NONE:
    case SECTION::OTHER:
    case SECTION::ASCII:
    default:
        throw std::logic_error("Delimited lines are for version, well, curve, and parameter section only.");
    }
}

void Las::AppendOtherData(const std::string& text)
{
    m_otherInformation += text;
}

void Las::AddAsciiLogData(const double& value)
{
    m_asciiLogData.push_back(value);
}

void Las::SetMinimumDepth(const double& depth)
{
    m_minimumDepth = depth;
}

void Las::SetMaximumDepth(const double& depth)
{
    m_maximumDepth = depth;
}

void Las::SetMaximumDataValue(const double& value)
{
    m_maximumDataValue = value;
}

void Las::SetMinimumDataValue(const double& value)
{
    m_minimumDataValue = value;
}

void Las::SetNullValue(const std::string& value)
{
    m_nullValue = value;
}

void Las::IncrementNumberOfCurves()
{
    ++m_numberOfCurves;
}

DelimitedLineSection const& Las::GetVersionInformation() const
{
    return m_versionInformation;
}

DelimitedLineSection const& Las::GetWellInformation() const
{
    return m_wellInformation;
}

DelimitedLineSection const& Las::GetCurveInformation() const
{
    return m_curveInformation;
}

DelimitedLineSection const& Las::GetParameterInformation() const
{
    return m_parameterInformation;
}

std::string const& Las::GetOtherInformation() const
{
    return m_otherInformation;
}

std::vector<double> const& Las::GetAsciiLogData() const
{
    return m_asciiLogData;
}

size_t Las::GetNumberOfCurves() const
{
    return m_numberOfCurves;
}

size_t Las::GetNumberOfCurveDetails() const
{
    return m_curveInformation.size();
}

double Las::GetMaximumDataValue() const
{
    return m_maximumDataValue;
}

double Las::GetMinimumDataValue() const
{
    return m_minimumDataValue;
}

double Las::GetMinimumDepth() const
{
    return m_minimumDepth;
}

double Las::GetMaximumDepth() const
{
    return m_maximumDepth;
}

std::string const& Las::GetNullValue() const
{
    return m_nullValue;
}

void LasParser::ParseDelimitedLine(
    std::string line, 
    DELIMITED_LINE& delimitedLine)
{
    size_t spacePosition{ std::string::npos };

    // Dot Position
    size_t delimiterPosition = line.find('.');

    if (delimiterPosition == std::string::npos)
    {
        throw std::logic_error("First delimeter '.' not found.");
    }

    // Find first space position after dot.
    for (size_t i = delimiterPosition + 1; i < line.length(); ++i)
    {
        if (line[i] == ' ') {
            spacePosition = i;
            break;
        }
    }

    if (spacePosition == std::string::npos)
    {
        throw std::logic_error("First space after '.' not found.");
    }

    std::string mnemonicAndUnit = line.substr(0, spacePosition);
    std::string dataAndDescription = line.substr(spacePosition + 1);

    // Get Mnemonic
    delimitedLine.m_mnemonic = mnemonicAndUnit.substr(0, delimiterPosition);
    StringHelpers::TrimWhitespaces(delimitedLine.m_mnemonic);

    // Get Unit
    delimitedLine.m_unit = mnemonicAndUnit.substr(delimiterPosition + 1);
    StringHelpers::TrimWhitespaces(delimitedLine.m_unit);

    delimiterPosition = dataAndDescription.find(':'); // Colon Position

    if (delimiterPosition == std::string::npos)
    {
        throw std::logic_error("Last delimeter ':' not found.");
    }

    // Get Data
    delimitedLine.m_data = dataAndDescription.substr(0, delimiterPosition);
    StringHelpers::TrimWhitespaces(delimitedLine.m_data);

    // Get Description
    delimitedLine.m_description = dataAndDescription.substr(delimiterPosition + 1);
    StringHelpers::TrimWhitespaces(delimitedLine.m_description);
}

Las* LasParser::Parse(
    const std::string& filename, 
    std::string& resultMessage)
{
    Las* las{ new Las() };

    std::string line{};
    SECTION currentSection{ SECTION::NONE };
    size_t dataIndex{ 0 };

    std::ifstream inputFile{};
    inputFile.open(filename);

    if (!inputFile.is_open())
    {
        delete las;
        las = nullptr;

        resultMessage = "ERROR: Failed to open LAS file.";
        return las;
    }

    // Get filename and extension.
    size_t lastSlashPos = filename.find_last_of("/\\");

    resultMessage = lastSlashPos != std::string::npos ?
        filename.substr(lastSlashPos + 1) : filename;

    size_t lineNumber{ 0 };

    // Read each line.
    while (std::getline(inputFile, line))
    {
        ++lineNumber;

        switch (line[0])
        {
        case '~': // Line is a header.
        {
            // Check Section.
            switch (line[1])
            {
            case 'V': // Version Information Section
                // If ~V is already populated.
                if (las->GetVersionInformation().size() > 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~V appeared again.";
                    return las;
                }

                // If ~V is not the first section.
                if (currentSection != SECTION::NONE)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~V must be the first section.";
                    return las;
                }

                currentSection = SECTION::VERSION;
                break;
            case 'W': // Well Information Section
                // If ~W is already populated.
                if (las->GetWellInformation().size() > 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~W appeared again.";
                    return las;
                }

                // If ~W is not appeared after ~V.
                if (currentSection != SECTION::VERSION)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~W must appear after ~V.";
                    return las;
                }

                // If ~V is empty.
                if (las->GetVersionInformation().size() == 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~V must not be empty.";
                    return las;
                }

                currentSection = SECTION::WELL;
                break;
            case 'C': // Curve Information Section
                // If ~C is already populated.
                if (las->GetCurveInformation().size() > 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~C appeared again.";
                    return las;
                }

                // If ~C is not appeared after ~W.
                if (currentSection != SECTION::WELL)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~C must appear after ~W.";
                    return las;
                }

                // If ~W is empty.
                if (las->GetWellInformation().size() == 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~W must not be empty.";
                    return las;
                }

                currentSection = SECTION::CURVE;
                break;
            case 'P': // Parameter Information Section
                // If ~P is already populated.
                if (las->GetParameterInformation().size() > 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~P appeared again.";
                    return las;
                }

                // If ~P is not appeared after ~C.
                if (currentSection != SECTION::CURVE)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~P must appear after ~C.";
                    return las;
                }

                // If ~C is empty.
                if (las->GetCurveInformation().size() == 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~C must not be empty.";
                    return las;
                }

                currentSection = SECTION::PARAMETER;
                break;
            case 'O': // Other Information Section
                // If ~O is already populated.
                if (!las->GetOtherInformation().empty())
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~O appeared again.";
                    return las;
                }

                // If ~O is not appeared after ~C or ~P.
                if (!(currentSection == SECTION::CURVE ||
                    currentSection == SECTION::PARAMETER))
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~O must appear after ~C or ~P.";
                    return las;
                }

                // If ~C or ~P is empty.
                if (currentSection == SECTION::CURVE &&
                    las->GetCurveInformation().size() == 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~C must not be empty.";
                    return las;
                }
                else if (currentSection == SECTION::PARAMETER &&
                        las->GetParameterInformation().size() == 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~P must not be empty.";
                    return las;
                }

                currentSection = SECTION::OTHER;
                break;
            case 'A': // ASCII Log Data Section
                // If ~A is already populated.
                if (las->GetAsciiLogData().size() > 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~A appeared again.";
                    return las;
                }

                // If ~A is not appeared after ~C, ~P, or ~O.
                if (!(currentSection == SECTION::CURVE ||
                    currentSection == SECTION::PARAMETER ||
                    currentSection == SECTION::OTHER))
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~A must be the last section.";

                    return las;
                }

                // If ~C, ~P, ~O is empty.
                if (currentSection == SECTION::CURVE &&
                    las->GetCurveInformation().size() == 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~C must not be empty.";
                    return las;
                }
                else if (currentSection == SECTION::PARAMETER &&
                    las->GetParameterInformation().size() == 0)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~P must not be empty.";
                    return las;
                }
                else if (currentSection == SECTION::OTHER &&
                    las->GetOtherInformation().empty())
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] ERROR: ~O must not be empty.";
                    return las;
                }

                currentSection = SECTION::ASCII;
                break;
            default: // Non-standard Section
                delete las;
                las = nullptr;

                resultMessage = "[Line " + std::to_string(lineNumber) + 
                    "] ERROR: Non-standard section occured.";
                return las;
            }

            break;
        }
        case '#': // Line is a comment.
        {
            break;
        }
        default:
        {
            if (line.empty() ||
                StringHelpers::IsWhitespaceString(line))
            {
                break;
            }

            switch (currentSection)
            {
            case SECTION::VERSION:
            case SECTION::CURVE:
            case SECTION::PARAMETER:
            {
                DELIMITED_LINE delimitedLine{};

                try
                {
                    ParseDelimitedLine(line, delimitedLine);
                }
                catch (const std::exception&)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) +
                        "] Non-delimited line occured.";
                    return las;
                }

                las->SetDelimitedLine(currentSection, delimitedLine);
                break;
            }
            case SECTION::WELL:
            {
                DELIMITED_LINE delimitedLine{};

                try
                {
                    ParseDelimitedLine(line, delimitedLine);
                }
                catch (const std::exception&)
                {
                    delete las;
                    las = nullptr;

                    resultMessage = "[Line " + std::to_string(lineNumber) + 
                        "] Non-delimited line occured.";
                    return las;
                }

                las->SetDelimitedLine(currentSection, delimitedLine);

                if (delimitedLine.m_mnemonic == "STRT")
                {
                    las->SetMinimumDepth(std::stod(delimitedLine.m_data));
                }
                else if (delimitedLine.m_mnemonic == "STOP")
                {
                    las->SetMaximumDepth(std::stod(delimitedLine.m_data));
                }
                else if (delimitedLine.m_mnemonic == "NULL")
                {
                    las->SetNullValue(delimitedLine.m_data);
                }

                break;
            }
            case SECTION::OTHER:
            {
                las->AppendOtherData(line);
                break;
            }
            case SECTION::ASCII:
            {
                // Replace NULL Values.
                StringHelpers::ReplaceValues(
                    line, "NULL", las->GetNullValue());

                std::istringstream stream{ line };
                double value{};

                

                while (stream >> std::fixed >> std::setprecision(6) >> value)
                {
                    las->AddAsciiLogData(value);

                    if (dataIndex != 0) // Skip Depth (First Column)
                    {
                        if (value > las->GetMaximumDataValue())
                            las->SetMaximumDataValue(value);

                        if (value < las->GetMinimumDataValue())
                            las->SetMinimumDataValue(value);
                    }

                    ++dataIndex;

                    // Count number of Curves from Modulo-n.
                    // Where n is number of lines in Curve Information.
                    if (dataIndex % las->GetCurveInformation().size() == 0)
                    {
                        las->IncrementNumberOfCurves();
                        dataIndex = 0; // Reset Data Index
                    }
                }

                break;
            }
            case SECTION::NONE:
            default:
                delete las;
                las = nullptr;

                resultMessage = "[Line " + std::to_string(lineNumber) + 
                    "] ERROR: Non-standard line appeared.";
                return las;
            }
        }
        }
    }

    inputFile.close();
    return las;
}
