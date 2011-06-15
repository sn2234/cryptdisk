
#pragma once

#include "Document.h"

#include "CryptDiskHelpers.h"

class ImagesModel : public Document
{
public:
	ImagesModel(void);
	virtual ~ImagesModel(void);

	void Refresh();
	void DismountImage(ULONG imageId, bool bForce);

	const std::vector<MountedImageInfo>& MountedImages() const { return m_mountedImages; }
private:
	std::vector<MountedImageInfo>	m_mountedImages;
};
