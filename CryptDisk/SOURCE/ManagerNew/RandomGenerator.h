
#pragma once
#include "PdhQuery.h"

struct RND_MOUSE_EVENT
{
	DWORD		position;	// 4
	DWORD		time;		// 2
};

struct RND_KEYBOARD_EVENT
{
	DWORD		vkCode;		// 2
	DWORD		scanCode;	// 2
	DWORD		flags;		// 1
	DWORD		time;		// 2
};

class RandomGenerator : CryptoLib::RandomGeneratorBase, boost::noncopyable
{
public:
	explicit RandomGenerator(const std::wstring& seedFileName);
	~RandomGenerator();

	bool IsInitialized() const  { return m_bSlowPollThreadInitialized && m_bFastPollThreadInitialized; }
	void LoadSeedFile();
	void SaveSeedFile();

	void AddInitialRandom();

	void AddTimer();

	void AddMouseEvent(RND_MOUSE_EVENT *pEvent);
	void AddKeyboardEvent(RND_KEYBOARD_EVENT *pEvent);

protected:
	void AddCryptoAPI();
	void SlowPollThread();
	void FastPollThread();

	void AddPdhQuery( const PdhQuery &query );
private:
	std::wstring						m_seedFileName;
	HCRYPTPROV							m_hProv;
	std::shared_ptr<boost::thread>		m_slowPollThread;
	std::shared_ptr<boost::thread>		m_fastPollThread;
	volatile bool						m_bSlowPollThreadInitialized;
	volatile bool						m_bFastPollThreadInitialized;
};
