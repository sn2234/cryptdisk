
#include "StdAfx.h"

#include "PdhCounter.h"
#include "winapi_exception.h"


PdhCounter::PdhCounter( HQUERY hQuery, LPCWSTR counterName )
	:m_hCounter(NULL)
{
	PDH_STATUS status;

	status = PdhAddCounterW(hQuery, counterName, NULL, &m_hCounter);

	if(status != ERROR_SUCCESS)
	{
		std::stringstream ss;
		ss << "Unable to add counter: " << counterName;
		throw winapi_exception(ss.str());
	}
}

PdhCounter::~PdhCounter(void)
{
	if(m_hCounter)
	{
		PdhRemoveCounter(m_hCounter);
	}
}

std::map<std::wstring, PDH_RAW_COUNTER> PdhCounter::RawValues() const
{
	std::map<std::wstring, PDH_RAW_COUNTER> val;

	DWORD bufferSize = 0;
	DWORD itemsCount = 0;
	PdhGetRawCounterArrayW(m_hCounter, &bufferSize, &itemsCount, NULL);

	if(bufferSize != 0)
	{
		BYTE* pBuffer = new BYTE[bufferSize+sizeof(wchar_t)];

		PdhGetRawCounterArrayW(m_hCounter, &bufferSize, &itemsCount, (PPDH_RAW_COUNTER_ITEM_W)pBuffer);

		std::for_each(reinterpret_cast<PPDH_RAW_COUNTER_ITEM_W>(pBuffer), reinterpret_cast<PPDH_RAW_COUNTER_ITEM_W>(pBuffer)+itemsCount,
			[&val](const PDH_RAW_COUNTER_ITEM_W& it){
				val.insert(std::make_pair(it.szName, it.RawValue));
		});

		delete[] pBuffer;
	}

	return val;
}

