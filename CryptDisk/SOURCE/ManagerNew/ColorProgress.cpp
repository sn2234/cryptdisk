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

// ColorProgress.cpp : implementation file
//

#include "stdafx.h"

#include "ColorProgress.h"


// CColorProgress

IMPLEMENT_DYNAMIC(CColorProgress, CProgressCtrl)

CColorProgress::CColorProgress()
{

}

CColorProgress::~CColorProgress()
{
}


BEGIN_MESSAGE_MAP(CColorProgress, CProgressCtrl)
END_MESSAGE_MAP()



// CColorProgress message handlers
int CColorProgress::SetPos(int nPos)
{
	int			progressRed,progressGreen;
	COLORREF	color;
	int			newPos;
	int			rangeMin, rangeMax;
	int			size;
	int			oldPos;

	oldPos=CProgressCtrl::SetPos(nPos);

	newPos=GetPos();
	GetRange(rangeMin, rangeMax);
	size=rangeMax-rangeMin;

	if(newPos<=size/2)
	{
		progressRed=255;
		progressGreen=(255*newPos/(size/2));
	}
	else
	{
		progressRed=255-(255*newPos)/(size/2);
		if(!progressRed)
		{
			progressRed=255;
		}
		progressGreen=255;
	}

	color=RGB(progressRed,progressGreen,0);

	SendMessage(PBM_SETBARCOLOR,0,color);

	return oldPos;
}
