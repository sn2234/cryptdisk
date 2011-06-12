
#pragma once

class winapi_exception : public std::runtime_error
{
public:
	winapi_exception(const std::string& userMessage)
		: m_error(GetLastError())
		, runtime_error(userMessage)
	{
		FormatError(userMessage);
	}

	winapi_exception(const std::string& userMessage, int error)
		: m_error(error)
		, runtime_error(userMessage)
	{
		FormatError(userMessage);
	}

	virtual ~winapi_exception(void) throw()
	{
	}

	virtual const char*  what  ()  const throw() { return m_message.c_str(); }
	unsigned int error ()  const throw() { return m_error; }

private:
	void FormatError(const std::string& userMessage)
	{
		LPSTR buffer = NULL;
		std::stringstream s;

		if(FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, m_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL))
		{
			s << "[" << userMessage << "] " << buffer << " (" << m_error << ")" ;

			m_message = s.str();

			LocalFree(buffer);
		}
	}

private:
	std::string m_message;
	int m_error;
};
