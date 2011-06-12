#pragma once

class PasswordBuilder
{
public:
	PasswordBuilder(const std::vector<std::wstring>& keyFilesList, const unsigned char *password, size_t passwordLength, ULONG keyFileDataLength=0x1000);
	~PasswordBuilder(void);

	unsigned char * Password() const { return m_password; }
	size_t PasswordLength() const { return m_passwordLength; }

private:
	unsigned char	*m_password;
	size_t			m_passwordLength;
};
