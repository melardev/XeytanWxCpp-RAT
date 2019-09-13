#pragma once
#include "Packet.h"
#include <wx/string.h>

class PacketPresentation : public Packet
{
public:

	PacketPresentation();
	PacketPresentation(const wxString& username, const wxString& pcName, const wxString& osName);
	~PacketPresentation();

	template <class Archive>
	void save(Archive& ar) const
	{
		std::string username = this->username.ToStdString();
		std::string pcName = this->pcName.ToStdString();
		std::string osName = this->osName.ToStdString();
		// TODO: make username.ToStdWstring() work
		ar(cereal::base_class<Packet>(this),
		   username, pcName, osName);
	}


	template <class Archive>
	void load(Archive& ar)
	{
		std::string userName, pcName, osName;
		ar(cereal::base_class<Packet>(this),
		   userName, pcName, osName);

		this->username = userName;
		this->osName = osName;
		this->pcName = pcName;
	}

	wxString username;
	wxString pcName;
	wxString osName;
};

CEREAL_REGISTER_TYPE(PacketPresentation);

CEREAL_REGISTER_POLYMORPHIC_RELATION(Packet, PacketPresentation);

// https://uscilab.github.io/cereal/serialization_functions.html
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(PacketPresentation, cereal::specialization::member_load_save)
