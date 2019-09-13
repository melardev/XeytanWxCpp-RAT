#pragma once
#include <functional>
#include <wx/thread.h>

class RunnableThread : public wxThread
{
public:

	RunnableThread(std::function<void*()> func) : wxThread(wxTHREAD_JOINABLE), m_Runnable(func)
	{
		m_Runnable = func;
	}


protected:
	virtual void* Entry() override
	{
		return m_Runnable();
	}

private:
	std::function<void*()> m_Runnable;
};
