
#pragma once

class SafeHandle
{
public:
	explicit SafeHandle(HANDLE h)
		:m_handle(h, &::CloseHandle)
	{}

	operator HANDLE() const { return m_handle.get(); }

private:
	boost::shared_ptr<void>	m_handle;
};
