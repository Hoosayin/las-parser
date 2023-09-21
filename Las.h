#pragma once

#include <string>
#include <vector>
#include <limits>

enum class SECTION 
{
	NONE,
	VERSION,
	WELL,
	CURVE,
	PARAMETER,
	OTHER,
	ASCII,
};

struct DELIMITED_LINE
{
	std::string m_mnemonic{};
	std::string	m_unit{};
	std::string m_data{};
	std::string m_description{};

	DELIMITED_LINE() = default;
	~DELIMITED_LINE() = default;

	DELIMITED_LINE(
		const std::string& mnemonic, 
		const std::string& unit, 
		const std::string& data,
		const std::string& description) :
		m_mnemonic(mnemonic), 
		m_unit(unit), 
		m_data(data),
		m_description(description)
	{
	}

	bool operator==(const DELIMITED_LINE& other) const
	{
		return (m_mnemonic == other.m_mnemonic &&
			m_unit == other.m_unit &&
			m_data == other.m_data &&
			m_description == other.m_description);
	}
};

using DelimitedLineSection = std::vector<DELIMITED_LINE>;

class Las 
{
private:
	DelimitedLineSection m_versionInformation{};
	DelimitedLineSection m_wellInformation{};
	DelimitedLineSection m_curveInformation{};
	DelimitedLineSection m_parameterInformation{};
	std::string m_otherInformation{};
	std::vector<double> m_asciiLogData{};

#ifdef max
#undef max
#endif

	double m_minimumDataValue{ std::numeric_limits<double>::max() };
	double m_maximumDataValue{ 0.0 };

	double m_minimumDepth{ 0.0 };
	double m_maximumDepth{ 0.0 };

	std::string m_nullValue{ "0.0" };
	size_t m_numberOfCurves{ 0 };

public:
	Las() = default;
	virtual ~Las() = default;

	void SetDelimitedLine(SECTION section, const DELIMITED_LINE& delimitedLine);
	void AppendOtherData(const std::string& text);
	void AddAsciiLogData(const double& value);

	void SetMinimumDepth(const double& depth);
	void SetMaximumDepth(const double& depth);

	void SetMaximumDataValue(const double& value);
	void SetMinimumDataValue(const double& value);

	void SetNullValue(const std::string& value);

	void IncrementNumberOfCurves();

	DelimitedLineSection const& GetVersionInformation() const;
	DelimitedLineSection const& GetWellInformation() const;
	DelimitedLineSection const& GetCurveInformation() const;
	DelimitedLineSection const& GetParameterInformation() const;
	std::string const& GetOtherInformation() const;
	std::vector<double> const& GetAsciiLogData() const;

	size_t GetNumberOfCurves() const;
	size_t GetNumberOfCurveDetails() const;

	double GetMaximumDataValue() const;
	double GetMinimumDataValue() const;

	double GetMinimumDepth() const;
	double GetMaximumDepth() const;

	std::string const& GetNullValue() const;
};

class LasParser
{
private:
	LasParser() = default;
	virtual ~LasParser() = default;

	static void ParseDelimitedLine(
		std::string line,
		DELIMITED_LINE& delimitedLine);
public:
	static Las* Parse(const std::string& filename, std::string& resultMessage);

};