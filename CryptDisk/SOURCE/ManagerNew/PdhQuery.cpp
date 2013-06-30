
#include "stdafx.h"

#include "PdhQuery.h"
#include "winapi_exception.h"

PdhQuery::PdhQuery()
	: m_hQuery(NULL)
{
	PDH_STATUS status;

	status = PdhOpenQuery(NULL, 0, &m_hQuery);
	if(status != ERROR_SUCCESS)
	{
		throw winapi_exception("Unable to create PDH query");
	}
}

PdhQuery::~PdhQuery()
{
	if(m_hQuery)
	{
		PdhCloseQuery(m_hQuery);
	}
}

void PdhQuery::AddCounter( LPCWSTR counterName )
{
	if(m_counters.find(counterName) == m_counters.end())
	{
		m_counters[counterName] = std::shared_ptr<PdhCounter>(new PdhCounter(m_hQuery, counterName));
	}
}
std::vector<std::wstring> PdhQuery::ExpandWildcardPath( const std::wstring& path )
{
	DWORD bufferSize = 0;

	std::vector<std::wstring::value_type> strBuff(256);

	std::vector<std::wstring> tmp;
	if (PdhExpandWildCardPathW(NULL, path.c_str(), &strBuff[0], &bufferSize, 0) == PDH_MORE_DATA)
	{
		strBuff.resize(bufferSize + 2, 0);
		std::fill(std::begin(strBuff), std::end(strBuff), 0);

		PdhExpandWildCardPathW(NULL, path.c_str(), &strBuff[0], &bufferSize, 0);
		for (const wchar_t* p = &strBuff[0]; wcslen(p); p += wcslen(p) + 1)
		{
			tmp.push_back(p);
		}
	}
	return tmp;
}
void PdhQuery::AddCounters( LPCWSTR counterName )
{
	if(PdhValidatePathW(counterName) == ERROR_SUCCESS)
	{
		AddCounter(counterName);
	}
	else
	{
		auto tmp = PdhQuery::ExpandWildcardPath(counterName);
		std::for_each(tmp.cbegin(), tmp.cend(), [this](const decltype(*tmp.cbegin())& it){
			AddCounter(it.c_str());
		});
	}
}
