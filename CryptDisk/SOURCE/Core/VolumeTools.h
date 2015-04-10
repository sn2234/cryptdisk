
#pragma once

struct LogicalDiskDesc
{
	std::string deviceId;
	unsigned int diskType;
	std::string capacity;
	std::string freeSpace;
	std::string volumeName;
};

struct PartitionDesc
{
	std::string deviceId;
	bool bootPartition;
	std::string name;
	bool primaryPartition;
	std::string capacity;
	int index;

	std::vector<LogicalDiskDesc> logicalDiscs;
};

struct PhysicalDiskDesc
{
	std::string deviceId;
	unsigned int partitionsCount;
	std::string capacity;
	std::string model;

	std::vector<PartitionDesc> partitions;
};

struct VolumeDesk
{
	std::string deviceId;
	std::string capacity;
	std::string driveLetter;
	unsigned int driveType;
	std::string fileSystem;
	std::vector<DISK_EXTENT> diskExtents;
};

class VolumeTools
{
public:
	VolumeTools();
	std::vector<VolumeDesk> enumVolumes();

	static std::wstring prepareVolumeName(const std::wstring& rawVolumeName);

	static std::string formatDriveType(unsigned int driveType);
private:
	CComPtr<IWbemServices> m_pIWbemServices;
};
