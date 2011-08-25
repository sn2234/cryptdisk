
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

class RandomGenerator : public CryptoLib::RandomGeneratorBase, private boost::noncopyable
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

	void InitRandomUI();

protected:
	void AddCryptoAPI();
	void SlowPollThread();
	void FastPollThread();

	void AddPdhQuery( const PdhQuery &query );

	static LRESULT CALLBACK MouseHookProc(int code,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK KeyboardHookProc(int code,WPARAM wParam,LPARAM lParam);
	HHOOK MouseHook() const { return m_hMouseHook; }
	HHOOK KeyboardHook() const { return m_hKeyboardHook; }

private:
	std::wstring						m_seedFileName;
	HCRYPTPROV							m_hProv;
	std::shared_ptr<boost::thread>		m_slowPollThread;
	std::shared_ptr<boost::thread>		m_fastPollThread;
	volatile bool						m_bSlowPollThreadInitialized;
	volatile bool						m_bFastPollThreadInitialized;

	HHOOK								m_hMouseHook;
	HHOOK								m_hKeyboardHook;
	bool								m_randomGuiInitialized;
};
