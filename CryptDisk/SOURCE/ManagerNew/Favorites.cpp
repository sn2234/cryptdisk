
#include "stdafx.h"
#include "Favorites.h"

#include <tinyxml2.h>
#include "CommonTools.h"

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

std::vector<FavoriteImage> Favorites::Load( const std::wstring& filePath )
{
	std::vector<FavoriteImage> tmp;

	fs::path p(filePath);

	tinyxml2::XMLDocument doc;

	doc.LoadFile(p.string().c_str());

	//	ticpp::Element* pFavNode = doc.FirstChildElement("Favorites")->FirstChildElement("Favorite");
	if(doc.FirstChildElement("Favorites") && doc.FirstChildElement("Favorites")->FirstChildElement("Favorite"))
	{
		for (tinyxml2::XMLElement* p = doc.FirstChildElement("Favorites")->FirstChildElement("Favorite");
			p;
			p = p->NextSiblingElement("Favorite"))
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

			FavoriteImage x(conv.from_bytes(p->FirstChildElement("ImagePath")->GetText()),
				conv.from_bytes(p->FirstChildElement("DriveLetter")->GetText()).at(0),
				_stricmp(p->FirstChildElement("ReadOnly")->GetText(), "true") == 0,
				_stricmp(p->FirstChildElement("Removable")->GetText(), "true") == 0,
				_stricmp(p->FirstChildElement("MountManager")->GetText(), "true") == 0,
				_stricmp(p->FirstChildElement("PreserveTimestamp")->GetText(), "true") == 0);

			tmp.push_back(x);
		}
	}

	return tmp;

}

void Favorites::Save( const std::wstring& filePath, const std::vector<FavoriteImage>& favorites )
{
	std::ofstream out(filePath);
	out << R"(<?xml version="1.0" encoding="UTF-8"?>)" << std::endl;
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
		out << "\t</Favorite>" << std::endl;
	});

	out << "</Favorites>" << std::endl;
}

std::wstring Favorites::PreparePath()
{
	return CommonTools::MakeAppDataPath(L"Favorites.xml");
}

FavoritesManager::FavoritesManager()
{
	fs::wpath file(Favorites::PreparePath());

	if(fs::exists(file))
	{
		m_favorites = Favorites::Load(file.wstring());
	}
}

FavoritesManager::~FavoritesManager()
{

}
