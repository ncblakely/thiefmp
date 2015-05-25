#pragma once

template<class Type> class SimpleQueue
{
public:
	struct Element
	{
		Type* pData;
		Element* next;

		Element(Type* Data)
		{
			pData = Data;
			next = NULL;
		}
		//~Element()
		//{
		//	SAFE_DELETE(pData);
		//}
	};

	SimpleQueue()
	{
		m_firstItem = NULL;
		m_lastItem = NULL;
		m_numItems = 0;
	}

	~SimpleQueue()
	{
		Clear();
	}

	void Clear()
	{
		if (m_firstItem && m_lastItem && m_numItems)
		{
			while (m_numItems)
			{
				Element* pFirst = m_firstItem;
				m_firstItem = m_firstItem->next;

				delete(pFirst->pData);
				delete(pFirst);

				m_numItems--;
			}
		}
	}

	void Push(Type* Data)
	{
		if (!Data)
			return;

		if (!m_firstItem)
		{
			m_firstItem = new Element(Data);
			m_lastItem = m_firstItem;
		}
		else
		{
			Element* element = new Element(Data);
			m_lastItem->next = element;
			m_lastItem = element;
		}

		m_numItems++;
	}

	Type* Pop()
	{
		if (!m_firstItem)
			return NULL;

		Element* pFirst = m_firstItem;
		Type* returnedData = pFirst->pData;

		if (pFirst->next)
			m_firstItem = pFirst->next;
		else
			m_firstItem = NULL;

		delete pFirst;

		m_numItems--;
		return returnedData;
	}

	int GetSize()
	{
		return m_numItems;
	}
	
protected:
	Element*	m_firstItem;
	Element*	m_lastItem;
	int				m_numItems;
};