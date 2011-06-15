#include "StdAfx.h"
#include "ImagesModel.h"

#include "DriverTools.h"

ImagesModel::ImagesModel(void)
{
}


ImagesModel::~ImagesModel(void)
{
}

void ImagesModel::Refresh()
{
	if (AppDriver::instance().getDriverControl())
	{
		m_mountedImages = CryptDiskHelpers::ListMountedImages(*AppDriver::instance().getDriverControl());

		UpdateViews();
	}
}

void ImagesModel::DismountImage( ULONG imageId, bool bForce )
{
	CryptDiskHelpers::UnmountImage(*AppDriver::instance().getDriverControl(), imageId, bForce);
}
