
#include "stdafx.h"

#include "VolumeTools.h"
#include "ComHelpers.h"

namespace bsys = boost::system;

namespace
{
	template<class T>
	std::vector<T> ExecWbemQuery(
		CComPtr<IWbemServices> service,
		CComBSTR queryString,
		std::function <T(CComPtr<IWbemClassObject>)> transformFn)
	{
		CComPtr<IEnumWbemClassObject> pIEnumObject;
		CComBSTR bstrQL(_T("WQL"));
		HResult hRes;

		hRes = service->ExecQuery
			(bstrQL, queryString, WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pIEnumObject);

		HResultSafe hr;
		std::vector<T> result;

		while (WBEM_S_NO_ERROR == hr)
		{
			CComPtr<IWbemClassObject> pIObject;
			ULONG uReturned = 0;
			hr = pIEnumObject->Next(WBEM_INFINITE, 1, &pIObject, &uReturned);

			if (SUCCEEDED(hr) && uReturned)
			{
				result.emplace_back(transformFn(pIObject));
			}
		}

		return result;
	}

	template<class T>
	std::string getStringParameter(T pClass, const TCHAR* paramName)
	{
		CComVariant var;
		CComBSTR bstrProp(paramName);
		HResult hRes;

		hRes = pClass->Get(bstrProp, 0, &var, NULL, NULL);

		_bstr_t nm = var.bstrVal;
		return !nm ? std::string() : (LPSTR) nm;
	}

	std::vector<DISK_EXTENT> getVolumeExtents(const std::string& volumeId)
	{
		std::string volumeIdModified(volumeId);

		auto pos = volumeIdModified.find("?");
		if (pos != std::string::npos)
		{
			volumeIdModified.replace(pos, 1, ".");
		}

		if (volumeIdModified[volumeIdModified.size() - 1] == '\\')
		{
			volumeIdModified.erase(volumeIdModified.size() - 1, 1);
		}

		// Open device handle
		HANDLE hDevice = ::CreateFileA(volumeIdModified.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
		}

		SCOPE_EXIT{ CloseHandle(hDevice); };

		// Request extents

		DWORD outBufferSize = sizeof(VOLUME_DISK_EXTENTS);
		void * pBuffer = new char[outBufferSize];

		SCOPE_EXIT{ delete [] pBuffer; };

		while (true)
		{
			DWORD bytesReturned = 0;
			if (::DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, nullptr, 0, pBuffer, outBufferSize, &bytesReturned, nullptr))
			{
				break;
			}

			if (GetLastError() != ERROR_MORE_DATA)
			{
				throw bsys::system_error(bsys::error_code(::GetLastError(), bsys::system_category()));
			}

			delete [] pBuffer;
			outBufferSize *= 2;
			pBuffer = new char[outBufferSize];
		}

		const VOLUME_DISK_EXTENTS* pExtents = static_cast <const VOLUME_DISK_EXTENTS *>(pBuffer);
		const DWORD numberOfExtents = pExtents->NumberOfDiskExtents;
		return numberOfExtents ?
			std::vector<DISK_EXTENT>(&pExtents->Extents[0], &pExtents->Extents[numberOfExtents]) :
			std::vector<DISK_EXTENT>();
	}

	std::vector<LogicalDiskDesc> getPartitionLogicalDisks(CComPtr<IWbemServices> service, std::string partitionId)
	{
		std::string query(
			(boost::format("ASSOCIATORS OF {Win32_DiskPartition.DeviceID='%1%'} WHERE AssocClass = Win32_LogicalDiskToPartition")
			% partitionId).str());

		return ExecWbemQuery<LogicalDiskDesc>(service, query.c_str(), [](CComPtr<IWbemClassObject> x)->LogicalDiskDesc
		{
			LogicalDiskDesc tmp;
			HResult hRes;

			tmp.deviceId = getStringParameter(x, _T("DeviceID"));
			tmp.capacity = getStringParameter(x, _T("Size"));
			tmp.freeSpace = getStringParameter(x, _T("FreeSpace"));
			tmp.volumeName = getStringParameter(x, _T("VolumeName"));

			{
				CComVariant var;
				CComBSTR bstrProp(_T("DriveType"));

				hRes = x->Get(bstrProp, 0, &var, NULL, NULL);

				tmp.diskType = var.uintVal;
			}

			return std::move(tmp);
		});
	}

	std::vector<PartitionDesc> getDiskPartitions(CComPtr<IWbemServices> service, std::string diskId)
	{
		std::string query(
			(boost::format("ASSOCIATORS OF {Win32_DiskDrive.DeviceID='%1%'} WHERE AssocClass = Win32_DiskDriveToDiskPartition") % diskId).str());

		return ExecWbemQuery<PartitionDesc>(service, query.c_str(), [service](CComPtr<IWbemClassObject> x) -> PartitionDesc
		{
			PartitionDesc tmp;
			HResult hRes;

			{
				CComVariant var;
				CComBSTR bstrProp(_T("BootPartition"));

				hRes = x->Get(bstrProp, 0, &var, NULL, NULL);

				tmp.bootPartition = var.boolVal != 0;
			}

			tmp.capacity = getStringParameter(x, _T("Size"));
			tmp.name = getStringParameter(x, _T("Name"));
			tmp.deviceId = getStringParameter(x, _T("DeviceID"));

			{
				CComVariant var;
				CComBSTR bstrProp(_T("PrimaryPartition"));

				hRes = x->Get(bstrProp, 0, &var, NULL, NULL);

				tmp.primaryPartition = var.boolVal != 0;
			}

			{
				CComVariant var;
				CComBSTR bstrProp(_T("Index"));

				hRes = x->Get(bstrProp, 0, &var, NULL, NULL);

				tmp.index = var.uintVal;
			}

			tmp.logicalDiscs = getPartitionLogicalDisks(service, tmp.deviceId);

			return tmp;
		});
	}

	std::wstring extractVolumeId(const std::wstring& rawVolumeName)
	{
		std::wregex volumePattern(L"Volume\\{.+?\\}");

		std::wsmatch mr;
		if (std::regex_search(rawVolumeName, mr, volumePattern))
		{
			return mr.cbegin()->str();
		}
		else
		{
			throw std::logic_error("Bad volume format");
		}
	}
}


VolumeTools::VolumeTools()
{
	HResult hRes;

	CComPtr<IWbemLocator> pIWbemLocator;

	CComBSTR bstrNamespace(_T("root\\cimv2"));

	hRes = pIWbemLocator.CoCreateInstance(CLSID_WbemAdministrativeLocator);

	hRes = pIWbemLocator->ConnectServer(bstrNamespace, NULL,
		NULL, NULL, 0, NULL, NULL, &m_pIWbemServices);

}

std::vector<VolumeDesk> VolumeTools::enumVolumes()
{
	std::vector<VolumeDesk> volumes = ExecWbemQuery<VolumeDesk>(m_pIWbemServices, "Select * from Win32_Volume",
		[](CComPtr<IWbemClassObject> x)->VolumeDesk {
		VolumeDesk tmp;
		HResult hRes;

		tmp.deviceId = getStringParameter(x, _T("DeviceID"));
		tmp.capacity = getStringParameter(x, _T("Capacity"));
		tmp.driveLetter = getStringParameter(x, _T("DriveLetter"));
		tmp.fileSystem = getStringParameter(x, _T("FileSystem"));

		// driveType
		{
			CComVariant var;
			CComBSTR bstrProp(_T("DriveType"));

			hRes = x->Get(bstrProp, 0, &var, NULL, NULL);

			tmp.driveType = var.uintVal;
		}

		try
		{
			tmp.diskExtents = getVolumeExtents(tmp.deviceId);
		}
		catch (const bsys::system_error&)
		{

		}

		return std::move(tmp);
	});

	return volumes;
}

std::wstring VolumeTools::prepareVolumeName(const std::wstring& rawVolumeName)
{
	return std::wstring(L"\\??\\") + extractVolumeId(rawVolumeName);
}

std::string VolumeTools::formatDriveType(unsigned int driveType)
{
	switch (driveType)
	{
	case 0:
		return "Unknown";
	case 1:
		return "No Root Directory";
	case 2:
		return "Removable Disk";
	case 3:
		return "Local Disk";
	case 4:
		return "Network Drive";
	case 5:
		return "Compact Disk";
	case 6:
		return "RAM Disk";
	default:
		return "Unknown";
	}
}
