#include "StdAfx.h"

#include "Common.h"
#include "Manager.h"
#include "CmdLineParser.h"

CCmdLineParser::CCmdLineParser(void)
{
}

CCmdLineParser::~CCmdLineParser(void)
{
}

void CCmdLineParser::Parse( int argc, TCHAR **argv )
{
	for(int i=1;i<argc;i++)
	{
		if(_tcscmp(argv[i], _T("-a")) == 0)
		{
			DismountAll();
		}
	}
}

void CCmdLineParser::DismountAll()
{
	if(!theApp.m_bDriverOk)
		return;

	DISKS_COUNT_INFO	countInfo;

	if(theApp.m_manager.GetDisksCount(&countInfo))
	{
		DISK_BASIC_INFO		*pDiskInfo;
		pDiskInfo=new DISK_BASIC_INFO[countInfo.DiskCount];

		theApp.m_manager.GetDisksInfo(pDiskInfo,
			sizeof(DISK_BASIC_INFO)*countInfo.DiskCount);

		BOOL				bRez=TRUE;

		for(UINT i=0;i<countInfo.DiskCount;i++)
		{
			DISK_DELETE_INFO	info;

			info.bForce=TRUE;
			info.DriveLetter=pDiskInfo[i].DriveLetter;

			bRez&=theApp.m_manager.UninstallDisk(&info);
		}

		if(bRez)
		{
			theApp.m_manager.UnloadDriver();
		}

		delete[] pDiskInfo;
	}
}
