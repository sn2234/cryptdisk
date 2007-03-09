#pragma once

class CCmdLineParser
{
public:
	CCmdLineParser(void);
	virtual ~CCmdLineParser(void);

	void Parse(int argc, TCHAR **argv);
protected:
	void DismountAll();
};
