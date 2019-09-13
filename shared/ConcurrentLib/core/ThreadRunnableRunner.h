#pragma once
#include <wx/thread.h>
#include <functional>

template <class Runnable>
class ThreadRunnableRunner : public wxThread
{
public:

	ThreadRunnableRunner(const std::_Binder<std::_Unforced, void(Runnable::*)(), Runnable*>& runnable)
		: runnable(runnable)
	{
	}

protected:
	void* Entry() override
	{
		runnable();
		return nullptr;
	}
	std::_Binder<std::_Unforced, void(Runnable::*)(), Runnable*> runnable;
};
