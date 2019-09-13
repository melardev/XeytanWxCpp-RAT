#pragma once
#include "Packet.h"
#include <wx/string.h>

typedef struct
{
	wxString path;
	uint64_t pid;
	wxString title;
	bool hasUi;

private:
	friend class cereal::access; // Needed for private serialization methods

	template <typename Archive>
	void save(Archive& ar) const
	{
		std::wstring filePath = path.ToStdWstring();
		std::wstring titleWString = title.ToStdWstring();

		ar(filePath, pid, hasUi, titleWString);
	}

	template <typename Archive>
	void load(Archive& ar)
	{
		std::wstring filePath, titleWString;

		ar & filePath & pid & hasUi & titleWString;
		// ar(filePath, pid, hasUi, titleWString);

		path = filePath;
		title = titleWString;
	}
} Process, *PProcess, *LPProcess;

class PacketProcess : public Packet
{
public:
	PacketProcess();

	~PacketProcess()
	{
	}

	/*
	template <typename Archive>
	void save(Archive& ar) const
	{
		ar(cereal::base_class<Packet>(this), processes);
	}

	template <typename Archive>
	void load(Archive& ar)
	{
		ar(cereal::base_class<Packet>(this), processes);
	}
	*/

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(cereal::base_class<Packet>(this), processes);
	}


	std::vector<Process> processes;
};


CEREAL_REGISTER_TYPE(PacketProcess);

// CEREAL_REGISTER_POLYMORPHIC_RELATION(Packet, PacketProcess);

// CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(PacketProcess, cereal::specialization::member_load_save)

