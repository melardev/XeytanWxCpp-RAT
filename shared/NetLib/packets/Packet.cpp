#include "Packet.h"


Packet::Packet()
{
}

Packet::Packet(PacketType packetType) : packetType(packetType)
{
}

Packet::~Packet()
{
	// std::cout << "Packet destructor called" << std::endl;
}
