#pragma once
#include <xstring>
#include <cereal/access.hpp>

struct FileInfo
{
	FileInfo();
	~FileInfo();

	uintmax_t size;
	std::wstring filePath;
	time_t lastUpdateTime;

private:
	friend class cereal::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & filePath;
		ar & lastUpdateTime;
		ar & size;
	}
};
