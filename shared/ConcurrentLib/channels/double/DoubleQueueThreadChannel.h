#pragma once
#include "IDoubleThreadChannel.h"
#include "../single/QueueThreadChannel.h"

template <class T>
class DoubleQueueThreadChannel : public IDoubleThreadChannel<T>
{
public:
	DoubleQueueThreadChannel(
		const wxSharedPtr<QueueThreadChannel<T>>& left,
		const wxSharedPtr<QueueThreadChannel<T>>& right);
	virtual ~DoubleQueueThreadChannel();
protected:
	void submitToLeft(const wxSharedPtr<T>& event) override;
	void submitToRight(const wxSharedPtr<T>& event) override;
	wxSharedPtr<T> takeFromLeft() override;
	wxSharedPtr<T> takeFromRight() override;

	wxSharedPtr<QueueThreadChannel<T>> getLeftThreadChannel();
	void setLeftThreadChannel(IThreadChannel<T>*);

	wxSharedPtr<QueueThreadChannel<T>> getRightThreadChannel();
	void setRightThreadChannel(IThreadChannel<T>*);

protected:
	wxSharedPtr<QueueThreadChannel<T>> leftThreadChannel;
	wxSharedPtr<QueueThreadChannel<T>> rightThreadChannel;
};


template <class T>
DoubleQueueThreadChannel<T>::DoubleQueueThreadChannel(
	const wxSharedPtr<QueueThreadChannel<T>>& left,
	const wxSharedPtr<QueueThreadChannel<T>>& right)
	:
	leftThreadChannel(left),
	rightThreadChannel(right)
{
}

template <class T>
DoubleQueueThreadChannel<T>::~DoubleQueueThreadChannel()
{
}


template <class T>
void DoubleQueueThreadChannel<T>::submitToLeft(const wxSharedPtr<T>& event)
{
	leftThreadChannel->submitAsync(event);
}

template <class T>
void DoubleQueueThreadChannel<T>::submitToRight(const wxSharedPtr<T>& event)
{
	rightThreadChannel->submitAsync(event);
}

template <class T>
wxSharedPtr<T> DoubleQueueThreadChannel<T>::takeFromLeft()
{
	return leftThreadChannel->takeSync();
}

template <class T>
wxSharedPtr<T> DoubleQueueThreadChannel<T>::takeFromRight()
{
	return rightThreadChannel->takeSync();
}

template <class T>
wxSharedPtr<QueueThreadChannel<T>> DoubleQueueThreadChannel<T>::getLeftThreadChannel()
{
	return leftThreadChannel;
}

template <class T>
void DoubleQueueThreadChannel<T>::setLeftThreadChannel(IThreadChannel<T>* channel)
{
	leftThreadChannel = channel;
}

template <class T>
wxSharedPtr<QueueThreadChannel<T>> DoubleQueueThreadChannel<T>::getRightThreadChannel()
{
	return rightThreadChannel;
}

template <class T>
void DoubleQueueThreadChannel<T>::setRightThreadChannel(IThreadChannel<T>* channel)
{
	rightThreadChannel = channel;
}
