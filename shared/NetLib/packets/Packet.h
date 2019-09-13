#pragma once
#include "packetdefs.h"

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

class Packet
{
public:
	Packet();
	Packet(PacketType packetType);
	virtual ~Packet();


	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(packetType);
	}

	/*
	template <class Archive>
	void save(Archive& ar) const
	{
		int16_t packetTypeInt = (int16_t)packetType;
		ar(packetTypeInt);
	}

	template <class Archive>
	void load(Archive& ar)
	{
		int16_t packetTypeInt;
		ar(packetTypeInt);
		packetType = (PacketType)packetTypeInt;
	}
	*/

	PacketType packetType;
};
