#pragma once
#include <wx/sharedptr.h>

template <class T>
class IThreadChannel
{
public:
	IThreadChannel();
	virtual ~IThreadChannel();
	virtual wxSharedPtr<T> takeSync() = 0;
	virtual void submitAsync(const wxSharedPtr<T>&) = 0;
};

template <class T>
IThreadChannel<T>::IThreadChannel()
{
}

template <class T>
IThreadChannel<T>::~IThreadChannel()
{
}
