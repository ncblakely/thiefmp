#pragma once

#include <queue>

struct NetMessage
{
	NetMessage()
	{
		m_pMsgData = NULL;
		m_size = 0;
		m_bufferHandle = -1;
		m_dpnidSender = 0;
	}

	void CopyData(BYTE* data, DWORD size)
	{
		m_pMsgData = new BYTE[size];
		memcpy(m_pMsgData, data, size);

		m_size = size;
	}

	void FreeData()
	{
		delete[] m_pMsgData;
		m_pMsgData = NULL;
	}

	BYTE* m_pMsgData;
	DWORD m_size;
	DPNHANDLE m_bufferHandle;
	DPNID m_dpnidSender;
};

class NetMessageQueue
{
public:
	void Add(NetMessage* msg)
	{
		m_queue.push(msg);
	}

	bool Empty()
	{
		return m_queue.empty();
	}

	NetMessage* GetNext()
	{
		NetMessage* msg = m_queue.front();
		m_queue.pop();

		return msg;
	}

	NetMessage* PeekNext()
	{
		return m_queue.front();
	}

protected:
	std::queue<NetMessage*> m_queue;
};