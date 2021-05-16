#pragma once
#include <Windows.h>

class Pusher
{
public:
	Pusher();
	~Pusher();
	void Close();
	void Push(int width, int height);

private:
	HANDLE m_hMapFile = NULL;
	void* m_data = nullptr;
	int m_width = -1;
	int m_height = -1;
};

