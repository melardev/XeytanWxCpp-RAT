#include "PacketPresentation.h"
#include "packetdefs.h"


PacketPresentation::PacketPresentation() : Packet(PacketType::Presentation)
{
}

PacketPresentation::PacketPresentation(const wxString& username, const wxString& pcName, const wxString& osName)
	: Packet(PacketType::Presentation), username(username), pcName(pcName), osName(osName)
{
}


PacketPresentation::~PacketPresentation()
{
}
