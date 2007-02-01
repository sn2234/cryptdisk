/*
* Copyright (c) 2006, nobody
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdafx.h"

#include "Common.h"

#include "resource.h"

#include "DlgRandom.h"
#include "resource.h"

#define	SEED_SIZE	(256*8)

#define MSG_HANDLER(X)	BOOL CALLBACK X(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#define HANDLE_MSG(X)	X(hDlg,msg,wParam,lParam)

CRandom				g_randomSource;
static	BOOL		Initialized;
static	BOOL		preInit;

static BOOL CALLBACK RandomDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK KeyboardHookProc(int code,WPARAM wParam,LPARAM lParam);
static LRESULT CALLBACK MouseHookProc(int code,WPARAM wParam,LPARAM lParam);
static void ShowProgress();
static void UnhookHooks();

static HHOOK	hMouseHook,hKeyboardHook;
static CDlgRandom *pDlg=NULL;

static int				seed_col;

static const TCHAR		seedFileName[]=_T("\\random.seed");

BOOL	RandomOnAppStart(HINSTANCE hInst)
{
	TCHAR	seedPath[MAX_PATH+1];

	Initialized=FALSE;
	preInit=FALSE;

	GetModuleFileName(hInst, seedPath, _countof(seedPath));
	
	TCHAR *ptr=NULL;

	for(int i=0;(i<sizeof(seedPath)) && seedPath[i];i++)
	{
		if(seedPath[i] == _T('\\'))
			ptr=&seedPath[i];
	}

	if(ptr)
	{
		*ptr=0;
		_tcscat_s(seedPath, seedFileName);
		g_randomSource.Init((TCHAR*)seedPath);
	}
	else
	{
		// Init random pool
		g_randomSource.Init((TCHAR*)seedFileName);
	}
	
	// Debug!
#ifdef _DEBUG
//	return TRUE;
#endif

	// Setup hooks
	if(hMouseHook=SetWindowsHookEx(WH_MOUSE_LL,&MouseHookProc,hInst,0))
	{
		if(hKeyboardHook=SetWindowsHookEx(WH_KEYBOARD_LL,&KeyboardHookProc,hInst,0))
		{
			preInit=TRUE;
		}
		else
		{
			UnhookWindowsHookEx(hMouseHook);
			hMouseHook=0;
		}
	}

	return preInit;
}

void	RandomOnAppClose()
{

	// Unhook hooks
	UnhookHooks();

	// Save seed
	g_randomSource.Cleanup();
}


BOOL InitRandom(HINSTANCE hInst, HWND hParent)
{
	if(!preInit)
	{
		return FALSE;
	}

	if(!Initialized)
	{
		seed_col=0;
		pDlg= new CDlgRandom;
		if(pDlg)
		{
			pDlg->SetMaxPos(SEED_SIZE);
			if(pDlg->DoModal() == IDOK)
			{
				Initialized=TRUE;
			}

			delete pDlg;
			pDlg=NULL;
		}
		seed_col=0;
	}

	return Initialized;
}

BOOL	IsInitialized()
{
	return Initialized;
}

static LRESULT CALLBACK MouseHookProc(int code,WPARAM wParam,LPARAM lParam)
{
	MSLLHOOKSTRUCT		*pData;
	RND_MOUSE_EVENT		mouseEvent;

	pData=(MSLLHOOKSTRUCT*)lParam;

	if( ! (pData->flags&LLMHF_INJECTED))	// Skip injected events
	{
		memset(&mouseEvent, 0, sizeof(mouseEvent));

		mouseEvent.position=LOWORD(pData->pt.x)|(LOWORD(pData->pt.y)<<16);
		mouseEvent.time=pData->time;
		g_randomSource.AddMouseEvent(&mouseEvent);
		seed_col++;
		ShowProgress();
	}
	return CallNextHookEx(hMouseHook,code,wParam,lParam);
}

static LRESULT CALLBACK KeyboardHookProc(int code,WPARAM wParam,LPARAM lParam)
{
	KBDLLHOOKSTRUCT		*pData;
	RND_KEYBOARD_EVENT	keyboardEvent;

	static	BYTE	PrevChar;

	pData=(KBDLLHOOKSTRUCT*)lParam;
	memset(&keyboardEvent, 0, sizeof(keyboardEvent));


	if((PrevChar!=pData->scanCode) && !(pData->flags&LLKHF_INJECTED))
	{
		keyboardEvent.flags=pData->flags;
		keyboardEvent.scanCode=pData->scanCode;
		keyboardEvent.time=pData->time;
		keyboardEvent.vkCode=pData->vkCode;

		PrevChar=LOBYTE(pData->scanCode);

		g_randomSource.AddKeyboardEvent(&keyboardEvent);

		seed_col+=2;
		ShowProgress();
	}

	return CallNextHookEx(hKeyboardHook,code,wParam,lParam);
}

static void UnhookHooks()
{
	if(hMouseHook)
	{
		UnhookWindowsHookEx(hMouseHook);
		hMouseHook=0;
	}
	if(hKeyboardHook)
	{
		UnhookWindowsHookEx(hKeyboardHook);
		hKeyboardHook=0;
	}
}

static void ShowProgress()
{

	if(!pDlg)
	{
		return;
	}

	pDlg->SetCurPos(seed_col);

	if(seed_col>=SEED_SIZE-1)
	{
		PostMessage(pDlg->GetSafeHwnd(), WM_COMMAND,IDOK, 0);
	}
}
