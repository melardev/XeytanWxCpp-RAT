#pragma once
#include <wx/sharedptr.h>

template <class T>
class IDoubleThreadChannel
{
public:
	IDoubleThreadChannel();
	virtual ~IDoubleThreadChannel();

protected:
	virtual void submitToLeft(const wxSharedPtr<T>& event) = 0;
	virtual void submitToRight(const wxSharedPtr<T>& event) = 0;
	virtual wxSharedPtr<T> takeFromLeft() = 0;
	virtual wxSharedPtr<T> takeFromRight() = 0;
};

template <class T>
IDoubleThreadChannel<T>::IDoubleThreadChannel()
{
}

template <class T>
IDoubleThreadChannel<T>::~IDoubleThreadChannel()
{
}
