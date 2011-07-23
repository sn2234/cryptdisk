
#pragma once

#include "PdhCounter.h"

class PdhQuery
{
public:
	PdhQuery();
	~PdhQuery();

	void AddCounter(LPCWSTR counterName);
	void AddCounters(LPCWSTR counterName);
	void CollectQueryData() { PdhCollectQueryData(m_hQuery); }

	const std::map<std::wstring, std::shared_ptr<PdhCounter>>& Counters() const { return m_counters; }
	static std::vector<std::wstring> ExpandWildcardPath(const std::wstring& path);
private:
	HQUERY		m_hQuery;
	std::map<std::wstring, std::shared_ptr<PdhCounter>>	m_counters;
};
