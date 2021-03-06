
// ManagerNew.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ManagerNew.h"
#include "ManagerNewDlg.h"
#include "AppRandom.h"
#include "AppFavorites.h"
#include "AppMemory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CManagerNewApp

BEGIN_MESSAGE_MAP(CManagerNewApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CManagerNewApp construction

CManagerNewApp::CManagerNewApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CManagerNewApp object

CManagerNewApp theApp;


// CManagerNewApp initialization

BOOL CManagerNewApp::InitInstance()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	SCOPE_EXIT{ CoUninitialize(); };
	CoInitializeSecurity(NULL, -1, NULL,
		NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
		EOAC_NONE, 0);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AppRandom::instance();
	AppFavorites::instance();
	AppMemory::instance();

	HANDLE hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	Concurrency::CurrentScheduler::Create( Concurrency::SchedulerPolicy() );
	Concurrency::CurrentScheduler::RegisterShutdownEvent( hEvent );


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	std::shared_ptr<CShellManager> pShellManager(new CShellManager);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	try
	{
		CManagerNewDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	}
	catch (const std::exception& e)
	{
		MessageBox(NULL,CString(e.what()), _T("Exception"), MB_ICONERROR | MB_OK);
	}

	Favorites::Save(Favorites::PreparePath(), AppFavorites::instance().Favorites());

	Concurrency::CurrentScheduler::Detach();
	WaitForSingleObject( hEvent, INFINITE );

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

