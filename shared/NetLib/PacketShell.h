#pragma once
#include "packets/Packet.h"
#include <wx/string.h>

class PacketShell : public Packet
{
public:


	PacketShell();
	PacketShell(ShellAction action, const wxString& data);

	PacketShell(wxString data);


	template <class Archive>
	void save(Archive& ar) const
	{
		std::string data = this->data.ToStdString();
		int8_t action = (int8_t)this->action;
		ar(cereal::base_class<Packet>(this), action, data);
	}


	template <class Archive>
	void load(Archive& ar)
	{
		int8_t action;
		std::string data;

		ar(cereal::base_class<Packet>(this),
		   action, data);

		this->action = (ShellAction)action;
		this->data = data;
	}

	wxString data;
	ShellAction action;
};

CEREAL_REGISTER_TYPE(PacketShell);

CEREAL_REGISTER_POLYMORPHIC_RELATION(Packet, PacketShell);

CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(PacketShell, cereal::specialization::member_load_save);
