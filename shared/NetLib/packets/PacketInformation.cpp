#include "PacketInformation.h"
#include <utility>

PacketInformation::PacketInformation(): Packet(PacketType::ClientInformation)
{
}

PacketInformation::PacketInformation(const wxString& pcName, const wxString& os, const wxString& username,
                                     std::map<std::string, std::string> environment)
	: Packet(PacketType::ClientInformation), username(username), pcName(pcName), osName(os), env(std::move(environment))
{
}
