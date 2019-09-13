#pragma once
#include <utility>
#include "../models/FileInfo.h"

class PacketFileSystem : public Packet
{
public:
	PacketFileSystem() : Packet(PacketType::FileSystem)
	{
	}

	PacketFileSystem(const wxString& path, std::vector<FileInfo> files)
		: Packet(PacketType::FileSystem), path(path), files(std::move(files))
	{
	}

	template <class Archive>
	void save(Archive& ar) const
	{
		std::string pathStr = this->path.ToStdString();

		ar(cereal::base_class<Packet>(this), pathStr, files);
	}

	template <class Archive>
	void load(Archive& ar)
	{
		std::string pathStr;
		ar(cereal::base_class<Packet>(this), pathStr, files);

		this->path = pathStr;
	}

	wxString path;
	std::vector<FileInfo> files;
};

CEREAL_REGISTER_TYPE(PacketFileSystem);

CEREAL_REGISTER_POLYMORPHIC_RELATION(Packet, PacketFileSystem);

// https://uscilab.github.io/cereal/serialization_functions.html
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(PacketFileSystem, cereal::specialization::member_load_save)
