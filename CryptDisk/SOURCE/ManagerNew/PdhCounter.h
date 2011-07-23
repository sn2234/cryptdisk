
#pragma once

class PdhCounter
{
public:
	explicit PdhCounter(HQUERY hQuery, LPCWSTR counterName);
	~PdhCounter(void);

	PDH_RAW_COUNTER RawValue() const
	{
		PDH_RAW_COUNTER val;

		PdhGetRawCounterValue(m_hCounter, NULL, &val);

		return val;
	}

	std::map<std::wstring, PDH_RAW_COUNTER> RawValues() const;
private:
	HCOUNTER	m_hCounter;
};
