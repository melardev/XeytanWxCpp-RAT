#pragma once
#include "IThreadChannel.h"
#include <queue>
#include <wx/thread.h>
#include <iostream>

template <class T>
class QueueThreadChannel : public IThreadChannel<T>
{
public:
	QueueThreadChannel();
	virtual ~QueueThreadChannel() = 0;

	wxSharedPtr<T> takeSync() override;
	void submitAsync(const wxSharedPtr<T>&) override;
protected:
	std::queue<wxSharedPtr<T>> queue_;
	wxCondition* cv;
	wxMutex* mu;
};

template <class T>
QueueThreadChannel<T>::QueueThreadChannel()
{
	mu = new wxMutex;
	cv = new wxCondition(*mu);
}

template <class T>
QueueThreadChannel<T>::~QueueThreadChannel()
{
	delete cv;
	delete mu;
}

template <class T>
wxSharedPtr<T> QueueThreadChannel<T>::takeSync()
{
	// wxMutexLocker lock(mu);
	if (queue_.size() > 0)
	{
		mu->Lock();
		auto event = queue_.front();
		queue_.pop();
		mu->Unlock();
		return event;
	}
	else
	{
		cv->Wait([&]() { return !queue_.empty(); });
		auto event = queue_.front();
		queue_.pop();
		return event;
	}
}

template <class T>
void QueueThreadChannel<T>::submitAsync(const wxSharedPtr<T>& event)
{
	wxMutexLocker lock(*mu);
	queue_.push(event);
	cv->Signal(); // Notify one worker
}
