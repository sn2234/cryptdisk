
#include "stdafx.h"

#include "VolumesModel.h"


void VolumesModel::Refresh()
{
	m_volumesCache = m_volumeTools.enumVolumes();
}
