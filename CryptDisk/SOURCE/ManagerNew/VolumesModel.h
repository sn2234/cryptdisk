
#pragma once

#include "Document.h"
#include "VolumeTools.h"

class VolumesModel : public Document
{
public:
	void Refresh();

	const std::vector<VolumeDesk>& getVolumes() const { return m_volumesCache; }

private:
	std::vector<VolumeDesk> m_volumesCache;
	VolumeTools				m_volumeTools;
};
