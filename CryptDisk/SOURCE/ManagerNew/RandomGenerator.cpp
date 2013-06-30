
#include "stdafx.h"

#include "winapi_exception.h"
#include "SafeHandle.h"
#include "RandomGenerator.h"
#include "AppRandom.h"
#include "DialogRandom.h"

const int SeedFileSize = 4096;

RandomGenerator::RandomGenerator(const std::wstring& seedFileName)
	: m_seedFileName(seedFileName)
	, m_hProv(0)
	, m_bSlowPollThreadInitialized(false)
	, m_bFastPollThreadInitialized(false)
	, m_hMouseHook(NULL)
	, m_hKeyboardHook(NULL)
	, m_randomGuiInitialized(false)
{
	LoadSeedFile();

	if((!CryptAcquireContext(&m_hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_SILENT))&&
		(!CryptAcquireContext(&m_hProv,NULL,NULL,PROV_RSA_FULL,CRYPT_SILENT|CRYPT_NEWKEYSET)))
	{
		m_hProv=0;
	}

	AddInitialRandom();

	m_slowPollThread.reset(new boost::thread(std::bind(&RandomGenerator::SlowPollThread, this)));
	m_fastPollThread.reset(new boost::thread(std::bind(&RandomGenerator::FastPollThread, this)));

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(NULL);

	m_hMouseHook=SetWindowsHookEx(WH_MOUSE, &MouseHookProc, hInst, GetCurrentThreadId());
	m_hKeyboardHook=SetWindowsHookEx(WH_KEYBOARD, &KeyboardHookProc, hInst, GetCurrentThreadId());
}

RandomGenerator::~RandomGenerator()
{
	try
	{
		SaveSeedFile();
	}
	catch (...)
	{
	}

	if(m_hProv)
	{
		CryptReleaseContext(m_hProv,0);
	}

	m_slowPollThread->interrupt();
	m_fastPollThread->interrupt();

	m_slowPollThread->join();
	m_fastPollThread->join();

	if(m_hMouseHook)
	{
		UnhookWindowsHookEx(m_hMouseHook);
	}
	if(m_hKeyboardHook)
	{
		UnhookWindowsHookEx(m_hKeyboardHook);
	}
}

void RandomGenerator::LoadSeedFile()
{
	Lock();

	SafeHandle hFile(CreateFileW(m_seedFileName.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));

	if(static_cast<HANDLE>(hFile) != INVALID_HANDLE_VALUE)
	{
		if(GetFileSize(hFile, NULL) >= SeedFileSize)
		{
			SafeHandle hMapping(CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, SeedFileSize, NULL));
			if(static_cast<HANDLE>(hMapping))
			{
				LPVOID	pData = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, SeedFileSize);
				if(pData)
				{
					AddSample(pData, SeedFileSize, SeedFileSize*8);

					UnmapViewOfFile(pData);
				}
			}
		}
	}

	Unlock();
}

void RandomGenerator::SaveSeedFile()
{

	Lock();

	SafeHandle hFile(CreateFile(m_seedFileName.c_str(), GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
	if(static_cast<HANDLE>(hFile) != INVALID_HANDLE_VALUE)
	{
		SafeHandle hMapping(CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, SeedFileSize, NULL));
		if(static_cast<HANDLE>(hMapping))
		{
			LPVOID	pData = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, SeedFileSize);
			if(pData)
			{
				BYTE	key[AES_KEY_SIZE];

				m_pool.GenKey(key);
				m_gen.Init(key);
				m_gen.Generate(pData, SeedFileSize);
				m_gen.Clear();

				UnmapViewOfFile(pData);
			}
		}
	}

	Unlock();
}

void RandomGenerator::AddInitialRandom()
{
#pragma region "System random"
	// Add system parameters
	DWORD_PTR	dwTemp;

	dwTemp=(DWORD_PTR)GetActiveWindow();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetCapture();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetClipboardOwner();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetClipboardViewer();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetCurrentProcessId();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetCurrentThreadId();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetDesktopWindow();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetFocus();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetInputState();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetMessagePos();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetMessageTime();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetOpenClipboardWindow();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetProcessHeap();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetProcessWindowStation();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);
	dwTemp=(DWORD_PTR)GetTickCount();
	AddSample(&dwTemp, sizeof(DWORD_PTR), 4);

	POINT	pt;

	GetCaretPos(&pt);
	AddSample(&pt, sizeof(pt), 4);
	GetCursorPos(&pt);
	AddSample(&pt, sizeof(pt), 4);

	MEMORYSTATUS	memstat;

	GlobalMemoryStatus(&memstat);
	AddSample(&memstat, sizeof(memstat), 6);

	HANDLE	hndl;
	struct
	{
		FILETIME	creation;
		FILETIME	exit;
		FILETIME	kernel;
		FILETIME	user;
	}times_info;

	hndl=GetCurrentThread();
	GetThreadTimes(hndl, &times_info.creation, &times_info.exit, &times_info.kernel,
		&times_info.user);
	AddSample(&times_info, sizeof(times_info), 12);
	hndl=GetCurrentProcess();
	GetProcessTimes(hndl, &times_info.creation, &times_info.exit, &times_info.kernel,
		&times_info.user);
	AddSample(&times_info, sizeof(times_info), 12);

	SIZE_T	minSize,maxSize;

	GetProcessWorkingSetSize(hndl, &minSize, &maxSize);
	AddSample(&minSize, sizeof(minSize), 4);
	AddSample(&maxSize, sizeof(maxSize), 4);

#pragma endregion 

	AddCryptoAPI();
}

void RandomGenerator::AddCryptoAPI()
{
	// Add data from MS CryptoAPI
	BYTE			rnd_buff[256];

	if(m_hProv)
	{
		CryptGenRandom(m_hProv, sizeof(rnd_buff), rnd_buff);

		AddSample(rnd_buff, sizeof(rnd_buff), sizeof(rnd_buff)*8);
	}
	RtlSecureZeroMemory(rnd_buff,sizeof(rnd_buff));
}

void RandomGenerator::AddMouseEvent( RND_MOUSE_EVENT *pEvent )
{
	AddSample(pEvent, sizeof(RND_MOUSE_EVENT), 6);

	//Add timer
	AddTimer();
}

void RandomGenerator::AddKeyboardEvent( RND_KEYBOARD_EVENT *pEvent )
{
	AddSample(pEvent, sizeof(RND_KEYBOARD_EVENT), 7);

	// Add timer
	AddTimer();
}

void RandomGenerator::SlowPollThread()
{
	PdhQuery query;

	// Initialize performance counters
	query.AddCounters(L"\\Cache\\*");
	query.AddCounters(L"\\ICMP\\*");
	query.AddCounters(L"\\ICMPv6\\*");
	query.AddCounters(L"\\IPv4\\*");
	query.AddCounters(L"\\IPv6\\*");
	query.AddCounters(L"\\Memory\\*");
	query.AddCounters(L"\\Network Interface(*)\\*"); // slow
	query.AddCounters(L"\\Objects\\*");
	query.AddCounters(L"\\Paging File(_Total)\\*"); // slow
	query.AddCounters(L"\\PhysicalDisk(_Total)\\*");
	query.AddCounters(L"\\Process(_Total)\\*");
	query.AddCounters(L"\\ReadyBoost Cache\\*");
	query.AddCounters(L"\\Search Indexer(*)\\*"); // slow
	query.AddCounters(L"\\Server\\*");
	query.AddCounters(L"\\Synchronization(_Total)\\*");
	query.AddCounters(L"\\System\\*");
	query.AddCounters(L"\\TCPv4\\*");
	query.AddCounters(L"\\TCPv6\\*");
	query.AddCounters(L"\\UDPv4\\*");
	query.AddCounters(L"\\UDPv6\\*");
	query.AddCounters(L"\\USB(*)\\*");

	while (true)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(10));

		// Add timer
		AddTimer();

		// Add MS Crypto API random
		AddCryptoAPI();

		// Add performance data

		query.CollectQueryData();

		AddPdhQuery(query);

		m_bSlowPollThreadInitialized = true;
	}
}

void RandomGenerator::FastPollThread()
{
	PdhQuery query;

	// Initialize performance counters
	query.AddCounters(L"\\Cache\\*");
	query.AddCounters(L"\\ICMP\\*");
	query.AddCounters(L"\\ICMPv6\\*");
	query.AddCounters(L"\\IPv4\\*");
	query.AddCounters(L"\\IPv6\\*");
	query.AddCounters(L"\\Memory\\*");
//	query.AddCounters(L"\\Network Interface(*)\\*"); // slow
	query.AddCounters(L"\\Objects\\*");
//	query.AddCounters(L"\\Paging File(_Total)\\*"); // slow
	query.AddCounters(L"\\PhysicalDisk(_Total)\\*");
	query.AddCounters(L"\\Process(_Total)\\*");
	query.AddCounters(L"\\ReadyBoost Cache\\*");
//	query.AddCounters(L"\\Search Indexer(*)\\*"); // slow
	query.AddCounters(L"\\Server\\*");
	query.AddCounters(L"\\Synchronization(_Total)\\*");
	query.AddCounters(L"\\System\\*");
	query.AddCounters(L"\\TCPv4\\*");
	query.AddCounters(L"\\TCPv6\\*");
	query.AddCounters(L"\\UDPv4\\*");
	query.AddCounters(L"\\UDPv6\\*");
	query.AddCounters(L"\\USB(*)\\*");
	

	while (true)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(500));

		// Add timer
		AddTimer();

		// Add performance data

		query.CollectQueryData();

		AddPdhQuery(query);

		m_bFastPollThreadInitialized = true;
	}
}

void RandomGenerator::AddTimer()
{
	LARGE_INTEGER	timestamp;
	if(!QueryPerformanceCounter(&timestamp))
	{
		timestamp.QuadPart = GetTickCount64();
	}

	AddSample(&timestamp, sizeof(timestamp), 2);
}

void RandomGenerator::AddPdhQuery( const PdhQuery &query )
{
	std::for_each(std::begin(query.Counters()), std::end(query.Counters()), [&](decltype(*query.Counters().cbegin()) counter){
		auto tmp = counter.second->RawValues();
		std::for_each(std::begin(tmp), std::end(tmp), [&](decltype(*tmp.cbegin()) val){
			AddSample(&val.second, sizeof(val.second), 4);
		});
	});
}

LRESULT CALLBACK RandomGenerator::MouseHookProc(int code,WPARAM wParam,LPARAM lParam)
{
	MOUSEHOOKSTRUCT		*pData;
	RND_MOUSE_EVENT		mouseEvent;

	pData=(MOUSEHOOKSTRUCT*)lParam;

	{
		memset(&mouseEvent, 0, sizeof(mouseEvent));

		mouseEvent.position=LOWORD(pData->pt.x)|(LOWORD(pData->pt.y)<<16);
		mouseEvent.time=(DWORD)(__rdtsc()&0xFFFFFFFF);
		AppRandom::instance().AddMouseEvent(&mouseEvent);
	}
	return CallNextHookEx(AppRandom::instance().MouseHook(),code,wParam,lParam);
}

LRESULT CALLBACK RandomGenerator::KeyboardHookProc(int code,WPARAM wParam,LPARAM lParam)
{
	RND_KEYBOARD_EVENT	keyboardEvent;

	static	WPARAM	PrevChar;

	memset(&keyboardEvent, 0, sizeof(keyboardEvent));


	if(PrevChar!=wParam)
	{
		keyboardEvent.flags = (DWORD)lParam;
		keyboardEvent.scanCode = (BYTE)((lParam>>16)&0xFF);
		keyboardEvent.time = (DWORD)(__rdtsc()&0xFFFFFFFF);
		keyboardEvent.vkCode = LOBYTE(wParam);

		PrevChar = wParam;

		AppRandom::instance().AddKeyboardEvent(&keyboardEvent);
	}

	return CallNextHookEx(AppRandom::instance().KeyboardHook(),code,wParam,lParam);
}

void RandomGenerator::InitRandomUI()
{
	if(!m_randomGuiInitialized)
	{
		DialogRandom dlg;

		m_randomGuiInitialized = (dlg.DoModal() == IDOK);
	}
}
