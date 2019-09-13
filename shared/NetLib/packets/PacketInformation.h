#pragma once
#include "Packet.h"
#include <wx/string.h>

class PacketInformation : public Packet
{
public:

	PacketInformation();
	PacketInformation(const wxString& pcName, const wxString& os, const wxString& username,
	                  std::map<std::string, std::string> environment);

	~PacketInformation() = default;


	template <class Archive>
	void save(Archive& ar) const
	{
		std::string username = this->username.ToStdString();
		std::string pcName = this->pcName.ToStdString();
		std::string osName = this->osName.ToStdString();

		// TODO: make username.ToStdWString() work

		ar(cereal::base_class<Packet>(this), env,
		   username, pcName, osName);
	}


	template <class Archive>
	void load(Archive& ar)
	{
		std::string userName, pcName, osName;
		ar(cereal::base_class<Packet>(this), env,
		   userName, pcName, osName);

		this->username = userName;
		this->osName = osName;
		this->pcName = pcName;
	}

	wxString username;
	wxString pcName;
	wxString osName;
	std::map<std::string, std::string> env;
};

CEREAL_REGISTER_TYPE(PacketInformation);

CEREAL_REGISTER_POLYMORPHIC_RELATION(Packet, PacketInformation);

// https://uscilab.github.io/cereal/serialization_functions.html
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(PacketInformation, cereal::specialization::member_load_save)
