
#include "stdafx.h"
#include "Favorites.h"

#include "ticpp/ticpp.h"

namespace fs = boost::filesystem;

FavoriteImage::FavoriteImage( const std::wstring& imagePath, wchar_t driveLetter, bool readOnly, bool removable, bool mountManager, bool preserveTimestamp )
	: m_imagePath(imagePath)
	, m_driveLetter(driveLetter)
	, m_readOnly(readOnly)
	, m_removable(removable)
	, m_mountManager(mountManager)
	, m_preserveTimestamp(preserveTimestamp)
{

}


std::vector<FavoriteImage> LoadFavorites( const std::wstring& filePath )
{
	std::vector<FavoriteImage> tmp;

	fs::path p(filePath);

	ticpp::Document doc;
	
	doc.LoadFile(p.string(), TIXML_ENCODING_UTF8);

//	ticpp::Element* pFavNode = doc.FirstChildElement("Favorites")->FirstChildElement("Favorite");

	for (ticpp::Element* p = doc.FirstChildElement("Favorites")->FirstChildElement("Favorite"); p; p = p->NextSiblingElement("Favorite", false))
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

		FavoriteImage x(conv.from_bytes(p->FirstChildElement("ImagePath")->Value()),
			conv.from_bytes(p->FirstChildElement("DriveLetter")->Value()).at(0),
			_stricmp(p->FirstChildElement("ReadOnly")->Value().c_str(), "true") == 0,
			_stricmp(p->FirstChildElement("Removable")->Value().c_str(), "true") == 0,
			_stricmp(p->FirstChildElement("MountManager")->Value().c_str(), "true") == 0,
			_stricmp(p->FirstChildElement("PreserveTimestamp")->Value().c_str(), "true") == 0);

		tmp.push_back(x);
	}

	return tmp;
}

void SaveFavorites( const std::wstring& filePath, const std::vector<FavoriteImage>& favorites )
{
	std::ofstream out(filePath);
	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	out << "<Favorites>" << std::endl;

	std::for_each(favorites.cbegin(), favorites.cend(), [&out](const FavoriteImage& i){
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		out << "\t<Favorite>" << std::endl;
		out << "\t\t<ImagePath>" << conv.to_bytes(i.ImagePath()) << "</ImagePath>" << std::endl;
		out << "\t\t<DriveLetter>" << conv.to_bytes(i.DriveLetter()) << "</DriveLetter>" << std::endl;
		out << "\t\t<ReadOnly>" << std::boolalpha << i.ReadOnly() << "</ReadOnly>" << std::endl;
		out << "\t\t<Removable>" << std::boolalpha << i.Removable() << "</Removable>" << std::endl;
		out << "\t\t<MountManager>" << std::boolalpha << i.MountManager() << "</MountManager>" << std::endl;
		out << "\t\t<PreserveTimestamp>" << std::boolalpha << i.PreserveTimestamp() << "</PreserveTimestamp>" << std::endl;
	});

	out << "</Favorites>" << std::endl;
}

