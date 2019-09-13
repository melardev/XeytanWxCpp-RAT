#include "PacketShell.h"

PacketShell::PacketShell() : Packet(PacketType::ReverseShell)
{
}

PacketShell::PacketShell(ShellAction action, const wxString& data)
	: Packet(PacketType::ReverseShell), data(data), action(action)
{
}

PacketShell::PacketShell(wxString data): data(data)
{
}
